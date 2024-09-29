
/*
Copyright (C) 2003 Matthias Braun <matze@braunis.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "Network/ClientSocket.hpp"

#include <string.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Classes/Network/NetworkInterface.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Core/NetworkGlobals.hpp"
#include "Util/Exception.hpp"
#include "Util/Log.hpp"
// #include "Classes/Network/NetworkServer.hpp"
// #include "Classes/PlayerState.hpp"

#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Network/Address.hpp"
#include "Util/Endian.hpp"
#include "Util/UtilInterface.hpp"

ClientSocket::ClientSocket(ClientSocketObserver *o,
                           const std::string &whole_servername)
    : observer(0),
      socket(0),
      sendpos(0),
      tempoffset(0),
      player_id(INVALID_PLAYER_ID) {
  try {
    //        proxy.setProxy(gameconfig->proxyserver,
    //                gameconfig->proxyserveruser,
    //                gameconfig->proxyserverpass);

    // resolve server name
    int port = NETPANZER_DEFAULT_PORT_TCP;
    std::string servername;
    //        const char *server= proxy.proxyserver != ""
    //                ? proxy.proxyserver.c_str() : whole_servername.c_str();
    UtilInterface::splitServerPort(whole_servername, servername, &port);

    network::Address serveraddr = network::Address::resolve(servername, port);

    socket = new network::TCPSocket(serveraddr, this);

#if 0
        // no proxy for now.
                if(proxy.proxyserver != "") {
                    proxy.sendProxyConnect(*socket, whole_servername);
                    LOGGER.info("%s connected via proxy %s",
                            whole_servername.c_str(),
                            proxy.proxyserver.c_str());
                }
#endif
    initId();
    observer = o;
  } catch (...) {
    if (socket) socket->destroy();
    throw;
  }
}

ClientSocket::ClientSocket(ClientSocketObserver *o)
    : observer(o),
      socket(nullptr),
      sendpos(0),
      tempoffset(0),
      player_id(INVALID_PLAYER_ID) {
  initId();
}

void ClientSocket::initId() {
  // static int curid=1;
  // id=curid++;
  //  init XOR key
  encryptKeySend = 0;
  encryptKeyRecv = 0;
  // encryptKeySendS = 0;
  // encryptKeyRecvS = 0;
  // recvf = false;

  if (NetworkState::status == _network_state_server)  // server only
  {
    // AAdevice reset vars
    mydatastrtime = 0;
    conn_end = 0;
    pre_conn_end = 0;
    packets_count = 0;
    lastPActTime0 = 0;
    commandBurst = 0;

    if (GameConfig::server_command_burst_limit < 0) {
      LOGGER.warning(
          "server.server_command_burst_limit=[%d] out of range, setting to "
          "default [18]!",
          GameConfig::server_command_burst_limit);
      GameConfig::server_command_burst_limit = 18;  // default
    }
  }
}

ClientSocket::~ClientSocket() {
  if (socket) socket->destroy();
}

void ClientSocket::sendMessage(const void *data, size_t size) {
  if (socket) {
    if (sendpos + size > sizeof(sendbuffer)) {
      observer->onClientDisconnected(
          this, sendpos ? "Send buffer full, need to disconnect"
                        : "Send data bigger than buffer, need to disconnect");
      return;
    }

    Uint16 *buf = (Uint16 *)(sendbuffer + sendpos);
    Uint16 sizeValue = htol16(size);
    memcpy(buf, &sizeValue, sizeof(Uint16));

    Uint8 *bufb = (Uint8 *)(sendbuffer + sendpos);

    memcpy(buf + 1, data, size);
    sendpos += size + 2;

    // Encoding

    memcpy(&encrypted, buf + 1, 1);
    memcpy(&encryptedb, bufb + 3, 1);
    encrypted2 = encrypted;
    encryptedb2 = encryptedb;

    encrypted2 ^= encryptKeySend;
    memcpy(buf + 1, &encrypted2, 1);
    encryptedb2 ^= encryptKeySend;
    memcpy(bufb + 3, &encryptedb2, 1);

    if (NetworkState::status == _network_state_server)  // server only
    {
      if (encrypted == 1 && encryptedb == 6) {
        memcpy(&key2, buf + 2, 1);
        encryptKeySend = key2;
      }

    } else {  // client side

      if (encrypted == 1 && encryptedb == 7) {
        memcpy(&key2, buf + 2, 1);
        encryptKeySend = key2;
      }
    }

    //        sendRemaining(); // lets send later
  }
}

void ClientSocket::sendRemaining() {
  if (socket && sendpos) {
    size_t s = socket->send(sendbuffer, sendpos);
    if (!s) {
      return;  // early bail out
    }

    if (s != sendpos) {
      memmove(sendbuffer, sendbuffer + s, sendpos - s);
      sendpos -= s;
    } else {
      sendpos = 0;
    }
  }
}

void ClientSocket::onDataReceived(network::TCPSocket *so, const char *data,
                                  const int len) {
  (void)so;
  int dataptr = 0;
  unsigned int remaining = len;
  Uint16 packetsize = 0;

  //    LOGGER.debug("Received [%s] from server [%s]", data,
  //    so->getAddress().getIP().c_str());

  while (remaining) {
    if (tempoffset < sizeof(Uint16)) {
      // copy the needed until netMessage
      unsigned int needsize = sizeof(Uint16) - tempoffset;
      unsigned int tocopy = (remaining > needsize) ? needsize : remaining;
      memcpy(tempbuffer + tempoffset, data + dataptr, tocopy);
      remaining -= tocopy;
      tempoffset += tocopy;
      dataptr += tocopy;
    }

    if (tempoffset < sizeof(Uint16)) break;  // no more data

    packetsize = ltoh16(*((Uint16 *)tempbuffer));

    std::string ipstring;
    ipstring = ClientSocket::getFullIPAddress();
    const char *cipstring = ipstring.c_str();

    if (packetsize < sizeof(NetMessage)) {
      LOGGER.debug("Received buggy packet size (< min) [%d]", packetsize);
      observer->onClientDisconnected(this, "Received buggy packet size (< min)");
      break;  // we are deleted
    }

    if (packetsize > _MULTI_PACKET_LIMIT)  // changed (was _MAX_NET_PACKET_SIZE)
    {
      LOGGER.debug("Received buggy packet size (> max) [from IP %s]",
                   cipstring);
      observer->onClientDisconnected(this, "Received buggy packet size (> max)");
      // hardClose();
      break;  // we are deleted
    }

    if ((tempoffset - 2 < packetsize) && remaining) {
      unsigned int needsize = packetsize - (tempoffset - 2);
      unsigned int tocopy = (remaining > needsize) ? needsize : remaining;
      memcpy(tempbuffer + tempoffset, data + dataptr, tocopy);
      remaining -= tocopy;
      tempoffset += tocopy;
      dataptr += tocopy;
    }

    if (tempoffset - 2 == packetsize) {
      // LOGGER.info("Packet Last %d", packetsize);

      // Decoding
      Uint8 *buf2 = (Uint8 *)(tempbuffer);

      popc_t = (Uint8)tempbuffer[2];
      popc_0b = popc_t;
      popc_t1 = (Uint8)tempbuffer[3];
      popc_1b = popc_t1;

      popc_0b ^= encryptKeyRecv;
      memcpy(buf2 + 2, &popc_0b, 1);
      popc_1b ^= encryptKeyRecv;
      memcpy(buf2 + 3, &popc_1b, 1);

      opcv_0 = (int)tempbuffer[2];
      opcv_1 = (int)tempbuffer[3];
      opcv_2 = (Uint8)tempbuffer[4];

      if (NetworkState::status != _network_state_server)  // clients only
      {
        if (opcv_0 == 1 && opcv_1 == 6) {
          encryptKeyRecv = opcv_2;
        }

      } else {  // server side

        if (opcv_0 == 1 && opcv_1 == 7) {
          encryptKeyRecv = opcv_2;

          // check if key sent is the same of received
          if (encryptKeySend != encryptKeyRecv) {
            LOGGER.debug(
                "Suspect attack detected [IP = %s] (network cheating)!",
                cipstring);
            // ChatInterface::serversay("Server blocked suspect attack
            // (external)!");
            observer->onClientDisconnected(this, "Network Manager striked!");
            hardClose();
            break;
          }
        }

        // XXX// start of anti-spam device

        //                LOGGER.info("Packet size=[%d]", packetsize);

        if (packetsize > 2) {
          currentPActTime = SDL_GetTicks();  // current time
          packetDelta = currentPActTime - lastPActTime0;
          LOGGER.debug("Packet delta=[%d]", packetDelta);

          // anti pre-spawn chat string attack (multiple temporary patches)
          if (packetsize == 304 && packets_count == 1 && pre_conn_end == 1) {
            // conn_pause.setTimeOut(10000);
            conn_end = 1;
          }

          if (packetsize == 39 && packets_count == 0) {
            // conn_pause.setTimeOut(10000);
            pre_conn_end = 1;
          }

          if (packets_count == 0 && packetsize != 39) {
            LOGGER.debug("Suspect attack detected [IP = %s] (pre-spawning)!",
                         cipstring);
            // ChatInterface::serversay("Server blocked suspect attack
            // (external)!");
            observer->onClientDisconnected(this, "Network Manager striked!");
            hardClose();
            break;
          }

          if (packets_count < 3) {
            // conn_pause.setTimeOut(10000);
            packets_count++;
          }

          // packet opc identifier
          // opc_0 = (int)data[dataptr];
          // opc_1 = (int)data[dataptr+1];
          // opc_2 = (int)data[dataptr+2];
          // LOGGER.debug("%d %d %d", opc_0,opc_1,opc_2);

          if ((opcv_0 == 10 && opcv_1 == 0 && opcv_2 == 3) ||
              (opcv_0 == 10 && opcv_1 == 0 && opcv_2 == 1) ||
              (opcv_0 == 3 &&
               opcv_1 == 2))  // chat, ally chat, flag update - removed ally req
                              // (opcv_0 == 3 && opcv_1 == 5) ||
          {
            if (conn_end == 0) {
              LOGGER.debug("Suspect attack detected [IP = %s] (pre-spawning)!",
                           cipstring);
              // ChatInterface::serversay("Server blocked suspect attack
              // (external)!");
              observer->onClientDisconnected(this, "Network Manager striked!");

              hardClose();
              break;
            }

            if (conn_end == 1 && packets_count == 2) {
              LOGGER.debug("Suspect attack detected [IP = %s] (pre-spawning)!",
                           cipstring);
              // ChatInterface::serversay("Server blocked suspect attack
              // (external)!");
              observer->onClientDisconnected(this, "Network Manager striked!");

              hardClose();
              break;
            }

            mydatastrc++;
            mydatastrtime0 = mydatastrtime;
            mydatastrtime = currentPActTime;
            if (mydatastrtime - mydatastrtime0 < ANTI_SPAM_LIMIT) {
              if (mydatastrc > 6)  // max 7 lines
              {
                LOGGER.debug(
                    "Anti-Spam terminated a connection [IP = %s] (chat abuse)",
                    cipstring);
                ChatInterface::serversay(
                    "Anti-Spam terminated a connection (chat abuse)!");
                observer->onClientDisconnected(this, "Network Manager striked!");

                hardClose();
                break;
              }
            } else {
              mydatastrc = 0;
            }
            // LOGGER.debug("Match!");
          }

          if (packetDelta < 125) {
            // moving a unit is size 30
            if (packetsize > 27) {
              commandBurst++;

              if (commandBurst >
                  GameConfig::server_command_burst_limit)  // too many
                                                           // consecutive bursts
                                                           // - player is
                                                           // cheating!
              {
                LOGGER.info(
                    "Suspect cheater terminated! Burst=[%d] Limit=[%d] IP=[%s]",
                    commandBurst, GameConfig::server_command_burst_limit,
                    cipstring);
                commandBurst = 0;
                ChatInterface::serversay(
                    "Suspected cheater kicked (too many commands)!");
                observer->onClientDisconnected(this, "Anti-cheating striked!");
                //                                hardClose();
                break;
              }
            } else {
              // don't reset command burst here! might be unrelated package to
              // hacking.
            }
          } else {
            commandBurst = 0;  // sent packet slowly, reset burst.
          }

          lastPActTime0 = currentPActTime;
        }

        // end of anti-spam device
      }

      EnqueueIncomingPacket(tempbuffer + sizeof(Uint16), packetsize, player_id,
                            this);
      tempoffset = 0;
    }
  }  // while
}

void ClientSocket::onConnected(network::TCPSocket *so) {
  LOGGER.debug("ClientSocket: connected, id=%d", id);
  socket = so;
  observer->onClientConnected(this);
}

void ClientSocket::onDisconnected(network::TCPSocket *s) {
  (void)s;
  LOGGER.debug("ClientSocket: Disconnected id=%d", id);
  socket = 0;
  observer->onClientDisconnected(this, "Network connection closed");
}

void ClientSocket::onSocketError(network::TCPSocket *so) {
  (void)so;
  LOGGER.warning("ClientSocket: Network connection error id=%d", id);
  socket = 0;
  observer->onClientDisconnected(this, "Network connection error");
}

std::string ClientSocket::getFullIPAddress() {
  std::stringstream ip;
  ip << socket->getAddress().getIP();
  ip << ':' << socket->getAddress().getPort();
  return ip.str();
}

std::string ClientSocket::getIPAddress() {
  return socket->getAddress().getIP();
}
