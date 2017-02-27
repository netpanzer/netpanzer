
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

#include <string>
#include <string.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>

#include "Util/Log.hpp"
#include "Util/Exception.hpp"

#include "Classes/Network/NetworkInterface.hpp"
#include "Core/NetworkGlobals.hpp"
#include "Network/ClientSocket.hpp"

#include "Classes/Network/NetworkState.hpp"
//#include "Classes/Network/NetworkServer.hpp"
//#include "Classes/PlayerState.hpp"

#include "Util/UtilInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Util/Endian.hpp"
#include "Network/Address.hpp"

#include "Interfaces/ChatInterface.hpp"

using namespace std;

ClientSocket::ClientSocket(ClientSocketObserver *o, const std::string& whole_servername)
    : observer(0), socket(0), sendpos(0), tempoffset(0), player_id(INVALID_PLAYER_ID)
{
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

        network::Address serveraddr
            = network::Address::resolve(servername, port);

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
    } catch(...) {
        if (socket)
            socket->destroy();
        throw;
    }
}
ClientSocket::ClientSocket(ClientSocketObserver *o)
    : observer(o), socket(0), sendpos(0), tempoffset(0), player_id(INVALID_PLAYER_ID)
{
    initId();
}

void
ClientSocket::initId()
{
    static int curid=1;
    id=curid++;
    // init XOR key
    encryptKeySendC = 0;
    encryptKeyRecvC = 0;
    encryptKeySendS = 0;
    encryptKeyRecvS = 0;
    recvf = false;

    if ( NetworkState::status == _network_state_server ) // server only
    {
    //AAdevice reset vars
    conn_end = 0; pre_conn_end = 0; packets_count = 0; lastPActTime0 = 0; commandBurst = 0; burstTime = 0; burstTime0 = 0; commandBurstLimit = 13;//slowdown = 0;


    if (GameConfig::game_anticheat < 1 || GameConfig::game_anticheat > 5)
    {
    GameConfig::game_anticheat = 3; // default
    }

    if (GameConfig::game_anticheat == 1)
    {
    commandBurstLimit = 10; // very strict
    } else if (GameConfig::game_anticheat == 2)
              {
               commandBurstLimit = 11; // strict
              } else if (GameConfig::game_anticheat == 3)
                        {
                         commandBurstLimit = 12; // normal
                        } else if (GameConfig::game_anticheat == 4)
                                  {
                                   commandBurstLimit = 13; // permissive
                                  } else {
                                          commandBurstLimit = 14; // pretty null
                                         }


    }
}

ClientSocket::~ClientSocket()
{
    if (socket)
        socket->destroy();
}




void ClientSocket::sendMessage(const void* data, size_t size)
{
    if ( socket )
    {
        if ( sendpos+size > sizeof(sendbuffer) )
        {
            observer->onClientDisconected(this, sendpos
                                                ?"Send buffer full, need to disconnect"
                                                :"Send data bigger than buffer, need to disconnect");
            return;
        }

        Uint16 *buf = (Uint16*)(sendbuffer+sendpos);
        *buf = htol16(size);

        Uint8 *bufb = (Uint8*)(sendbuffer+sendpos);

        memcpy(buf+1, data, size);
        sendpos += size+2;

        // Encrypt Sending

        memcpy(&encrypted,buf+1,1);
        memcpy(&encryptedb,bufb+3,1);
        encrypted2 = encrypted;



        if ( NetworkState::status == _network_state_server )  // server only
        {

        encrypted2 ^= encryptKeySendS;
        //LOGGER.debug("Byte sent is [%d]; XOR is [%d];", encrypted, encrypted2);
        memcpy(buf+1,&encrypted2,1);


        if (recvf) {
            encryptKeyRecvS = encryptKeySendS;
        }

        if (encrypted == 1) {
        if (encryptedb == 6) {

        memcpy(&key2,buf+2,1);
        memcpy(&key0,buf,1);

        //LOGGER.debug("Sent KeyMSG [%d] (%d)[%d][%d][%d];", key2, key0, encrypted, encryptedb, key2);

        encryptKeySendS = key2; recvf = false;

        }
        }


        }




        if ( NetworkState::status == _network_state_client || NetworkState::status == _network_state_bot ) // client only
        {

        encrypted2 ^= encryptKeySendC;
        //LOGGER.debug("Byte sent is [%d]; XOR is [%d];", encrypted, encrypted2);
        memcpy(buf+1,&encrypted2,1);

        }



//        sendRemaining(); // lets send later
    }
}






void
ClientSocket::sendRemaining()
{
    if ( socket && sendpos )
    {
        size_t s = socket->send(sendbuffer, sendpos);
        if ( !s )
        {
            return; // early bail out
        }

        if ( s != sendpos )
        {
            memmove(sendbuffer, sendbuffer+s, sendpos-s);
            sendpos -= s;
        }
        else
        {
            sendpos = 0;
        }
    }
}

void
ClientSocket::onDataReceived(network::TCPSocket * so, const char *data, const int len)
{
    (void)so;
    int dataptr = 0;
    unsigned int remaining = len;
    Uint16 packetsize=0;




    while ( remaining )
    {
        if ( tempoffset < sizeof(Uint16) )
        {
            // copy the needed until netMessage
            unsigned int needsize = sizeof(Uint16)-tempoffset;
            unsigned int tocopy   = (remaining>needsize)?needsize:remaining;
            memcpy(tempbuffer+tempoffset, data+dataptr, tocopy);
            remaining  -= tocopy;
            tempoffset += tocopy;
            dataptr    += tocopy;
        }

        if ( tempoffset < sizeof(Uint16) )
            break; // no more data

        packetsize=ltoh16(*((Uint16*)tempbuffer));

        std::string ipstring;
        ipstring = ClientSocket::getFullIPAddress();
        const char * cipstring = ipstring.c_str();

        if ( packetsize < sizeof(NetMessage) )
        {
            LOGGER.debug("Received buggy packet size (< min) [%d]", packetsize);
            observer->onClientDisconected(this, "Received buggy packet size (< min)");
            break; // we are deleted
        }

        if ( packetsize > _MULTI_PACKET_LIMIT )  // changed (was _MAX_NET_PACKET_SIZE)
        {

            LOGGER.debug("Received buggy packet size (> max) [from IP %s]", cipstring);
            observer->onClientDisconected(this, "Received buggy packet size (> max)");
            //hardClose();
            break; // we are deleted
        }








        // fu's Anti-Spam/Cheat device

        if ( NetworkState::status == _network_state_server ) // server only
        {
        //LOGGER.info("Packet %d", packetsize);

        if ( packetsize > 2 )
        {

        currentPActTime = SDL_GetTicks(); // current time

        packetDelta = currentPActTime-lastPActTime0;


        // anti pre-spawn chat string attack (multiple temporary patches)
        if ( packetsize == 303 && packets_count == 1 && pre_conn_end == 1)
        {
         //conn_pause.setTimeOut(10000);
         conn_end = 1;
        }

        if ( packetsize == 39 && packets_count == 0)
        {
         //conn_pause.setTimeOut(10000);
         pre_conn_end = 1;
        }

        if (packets_count == 0 && packetsize != 39)
        {
         LOGGER.debug("Suspect attack detected [IP = %s] (pre-spawning)!", cipstring);
         //ChatInterface::serversay("Server blocked suspect attack (external)!");
         observer->onClientDisconected(this, "Network Manager striked!");
         hardClose();
         break;
        }

        if ( packets_count < 3 )
        {
         //conn_pause.setTimeOut(10000);
         packets_count++;
        }

        //packet opc identifier
        opc_0 = (int)data[dataptr];
        opc_1 = (int)data[dataptr+1];
        opc_2 = (int)data[dataptr+2];
        //LOGGER.debug("%d %d %d", opc_0,opc_1,opc_2);

        if ( (opc_0 == 10 && opc_1 == 0 && opc_2 == 3) || (opc_0 == 10 && opc_1 == 0 && opc_2 == 1) || (opc_0 == 3 && opc_1 == 5) ) // chat, ally chat, ally request
        {


        if (conn_end == 0)
        {
         LOGGER.debug("Suspect attack detected [IP = %s] (pre-spawning)!", cipstring);
         //ChatInterface::serversay("Server blocked suspect attack (external)!");
         observer->onClientDisconected(this, "Network Manager striked!");

         hardClose();
         break;
        }

        if (conn_end == 1 && packets_count == 2)
        {
         LOGGER.debug("Suspect attack detected [IP = %s] (pre-spawning)!", cipstring);
         //ChatInterface::serversay("Server blocked suspect attack (external)!");
         observer->onClientDisconected(this, "Network Manager striked!");

         hardClose();
         break;
        }



        mydatastrc++;
        mydatastrtime0 = mydatastrtime;
        mydatastrtime = currentPActTime;
        if (mydatastrtime - mydatastrtime0 < 700)
        {
         if (mydatastrc>6) // max 7 lines
         {
          LOGGER.debug("Anti-Spam terminated a connection [IP = %s] (chat abuse)", cipstring);
          ChatInterface::serversay("Anti-Spam terminated a connection (chat abuse)!");
          observer->onClientDisconected(this, "Network Manager striked!");

          hardClose();
          break;
         }
        }
        else
        {
         mydatastrc = 0;
        }
        //LOGGER.debug("Match!");
        }



        if (packetDelta<125 && packetDelta>115 && packetsize>27)
        {

         commandBurst++;

         burstTime0 = burstTime;
         burstTime = currentPActTime;
         burstDelta = burstTime - burstTime0;

         //LOGGER.debug("Burst from [%s] - pDelta = %u - bCount = %u - bDelta = %u", cipstring, packetDelta, commandBurst, burstDelta);
         LOGGER.debug("Burst from [%s] - bCount = %u - bDelta = %u", cipstring, commandBurst, burstDelta);


         if (burstDelta<800) // Does it depend on tanks number? It seems not.
         {

         if ( commandBurst>commandBurstLimit ) // (7-8-10-12-14) too many consecutive bursts - player is cheating!
         {
         commandBurst = 0;
         LOGGER.debug("Suspect cheater terminated! [IP = %s]", cipstring);
         ChatInterface::serversay("Suspect cheater terminated (too fast clicking)!");
         observer->onClientDisconected(this, "Anti-cheating striked!");

         //hardClose();
         break;
         }

         }
         else
         {
         commandBurst = 0;
         }


        }



        //lastPActTime2 = lastPActTime1;
        lastPActTime1 = lastPActTime0;
        lastPActTime0 = currentPActTime;

        }

        }

        // end of Anti-Spam/Cheat device










        if ( (tempoffset-2 < packetsize) && remaining )
        {
            unsigned int needsize = packetsize-(tempoffset-2);
            unsigned int tocopy   = (remaining>needsize)?needsize:remaining;
            memcpy(tempbuffer+tempoffset, data+dataptr, tocopy);
            remaining  -= tocopy;
            tempoffset += tocopy;
            dataptr    += tocopy;
        }

        if ( tempoffset-2 == packetsize )
        {
            //LOGGER.info("Packet Last %d", packetsize);
            // Encrypt Receiving
            Uint8 *buf2 = (Uint8*)(tempbuffer);
            popc_t = (Uint8)tempbuffer[2];
            popc_0b = popc_t;


            if ( NetworkState::status == _network_state_client || NetworkState::status == _network_state_bot ) // client only
            {

            popc_0b ^= encryptKeyRecvC;
            //LOGGER.debug("Byte recv is [%d]; XOR is [%d];", popc_t, popc_0b);
            memcpy(buf2+2,&popc_0b,1);

            opcv_0 = (int)tempbuffer[2];
            opcv_1 = (int)tempbuffer[3];
            opcv_2 = (Uint8)tempbuffer[4];
            //LOGGER.debug("%d %d %d", opcv_0,opcv_1,opcv_2);
            if (opcv_0 == 1 && opcv_1 == 6) {
            //LOGGER.debug("Got new key [%d]", opcv_2);
            }
            if (opcv_0 == 1 && opcv_1 == 6) {

               recvf = true; encryptKeyRecvC = opcv_2;
            }

            }



            if ( NetworkState::status == _network_state_server ) // server only
            {

            if (recvf) {
                encryptKeyRecvS = encryptKeyRecvC; encryptKeySendC = encryptKeyRecvC;
            }

            popc_0b ^= encryptKeyRecvS;
            //LOGGER.debug("Byte recv is [%d]; XOR is [%d];", popc_t, popc_0b);
            memcpy(buf2+2,&popc_0b,1);

            }



            EnqueueIncomingPacket(tempbuffer+sizeof(Uint16), packetsize, player_id, this);
            tempoffset = 0;
        }




    } // while
}




void
ClientSocket::onConnected(network::TCPSocket *so)
{
    LOGGER.debug("ClientSocket: connected, id=%d", id);
    socket = so;
    observer->onClientConnected(this);
}

void
ClientSocket::onDisconected(network::TCPSocket *so)
{
    (void)so;
    LOGGER.debug("ClientSocket: Disconected id=%d", id);
    socket=0;
    observer->onClientDisconected(this, "Network connection closed");
}

void
ClientSocket::onSocketError(network::TCPSocket *so)
{
    (void)so;
    LOGGER.warning("ClientSocket: Network connection error id=%d", id);
    socket=0;
    observer->onClientDisconected(this, "Network connection error");
}

std::string
ClientSocket::getFullIPAddress()
{
    std::stringstream ip;
    ip << socket->getAddress().getIP();
    ip << ':' << socket->getAddress().getPort();
    return ip.str();
}

std::string
ClientSocket::getIPAddress()
{
    return socket->getAddress().getIP();
}
