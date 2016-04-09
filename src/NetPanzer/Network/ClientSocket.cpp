
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

#include "Util/Log.hpp"
#include "Util/Exception.hpp"
#include "Classes/Network/NetworkInterface.hpp"
#include "Core/NetworkGlobals.hpp"
#include "Network/ClientSocket.hpp"
#include "Util/UtilInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Util/Endian.hpp"
#include "Network/Address.hpp"

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
        memcpy(buf+1, data, size);
        sendpos += size+2;
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

        packetsize=ltoh16(*((Uint16*)tempbuffer));;

        if ( packetsize < sizeof(NetMessage) )
        {
            LOGGER.debug("Received buggy packet size (< min) [%d]", packetsize);
            observer->onClientDisconected(this, "Received buggy packet size (< min)");
            break; // we are deleted
        }

        if ( packetsize > _MAX_NET_PACKET_SIZE )
        {
            LOGGER.debug("Received buggy packet size (> max) [%d]", packetsize);
            observer->onClientDisconected(this, "Received buggy packet size (> max)");
            break; // we are deleted
        }

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
