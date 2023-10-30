/*
Copyright (C) 2004 by Matthias Braun <matze@braunis.de>

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


#include <iostream>

#include <sstream>
#include <string.h>

#include "NetworkException.hpp"
#include "SocketHeaders.hpp"
#include "SocketBase.hpp"
#include "SocketManager.hpp"
#include "NetworkManager.hpp"
#include "Util/Log.hpp"

namespace network
{

const char * SocketBase::state_str[] =
{
    "ST_ERROR",
    "UNINITIALIZED",
    "RESOLVING",
    "RESOLVED",
    "CREATED",
    "CONFIGURED",
    "BOUND",
    "LISTENING",
    "CONNECTING",
    "CONNECTED",
    "DESTROYING"
};

SocketBase::SocketBase()
{
    lastError = 0;
    sockfd = NULL_SOCKET;
    state = UNINITIALIZED;
    disconnectTimer.setTimeOut(500);
}

SocketBase::SocketBase(const Address &a, bool isTcp)
    : addr(a)
{
    lastError = 0;
    sockfd = NULL_SOCKET;
    state = RESOLVED;
    create();
    setNonBlocking();
    disconnectTimer.setTimeOut(500);
    state = CONFIGURED;
}

SocketBase::SocketBase(SOCKET fd, const Address &a)
    : sockfd(fd), addr(a)
{
    lastError = 0;
    state = CONNECTED;
    SocketManager::addSocket(this);
    setNonBlocking();
    disconnectTimer.setTimeOut(500);
}

SocketBase::~SocketBase()
{
    // XXX this might be needed if sockets doesn't close nicely
//    int tmp;
//    while ( recv(sockfd, (char*)&tmp, sizeof(tmp), RECV_FLAGS) > 0 ) ; // read until there is no more.
    if (sockfd > -1) {
        shutdown(sockfd, SHUTDOWN_BOTH);
        closesocket(sockfd);
    }
}

void
SocketBase::setAddress(const Address &a)
{
    addr = a;
    state = RESOLVING;
    SocketManager::addSocket(this);
}

void
SocketBase::create ()
{
    if ( state == RESOLVED )
    {
        sockfd = socket(PF_INET, addr.socktype, addr.protocol);

        LOGGER.debug("SocketBase:: Create [%s:%d] socket", (addr.socktype == SOCK_STREAM)?"tcp":"udp",sockfd);

        if(sockfd == INVALID_SOCKET)
        {
            lastError = GET_NET_ERROR();
            std::stringstream msg;
            msg << "Couldn't create socket: " << NETSTRERROR(lastError);
            throw NetworkException(msg.str());
        }
        state = CREATED;
    }
    else
    {
        LOGGER.warning("Trying to recreate a socket [%s]", getStateString());
    }
}

void
SocketBase::setNonBlocking()
{
    if ( state >= CREATED )
    {
        int res;
#ifdef _WIN32
        unsigned long mode = 1;
        res = ioctlsocket(sockfd, FIONBIO, &mode);
#else
        res = fcntl(sockfd, F_SETFL, O_NONBLOCK);
#endif
        if ( res == SOCKET_ERROR ) {
            lastError = GET_NET_ERROR();
    //        doClose();
            std::stringstream msg;
            msg << "Couldn't set socket to nonblocking mode: " << NETSTRERROR(lastError);
            LOGGER.warning("%s", msg.str().c_str());
        }
    }
    else
    {
        LOGGER.warning("Trying to configure uncreated socket [%s]", getStateString());
    }
}

void
SocketBase::bindSocketTo(const Address& toaddr)
{
    if ( state == CONFIGURED )
    {
        int res = bind(sockfd, toaddr.getSockaddr(), toaddr.getSockaddrLen());
        if(res == SOCKET_ERROR) {
            lastError = GET_NET_ERROR();
            doClose();
            std::stringstream msg;
            msg << "Couldn't bind socket to address '"
                << toaddr.getIP() << "' port " << toaddr.getPort()
                << ": " << NETSTRERROR(lastError);
            throw NetworkException(msg.str());
        }
        SocketManager::addSocket(this);
        state = BOUND;
    }
    else
    {
        LOGGER.warning("Trying to bind to a socket != CONFIGURED [%s]", getStateString());
    }
}

void
SocketBase::setReuseAddr()
{
    if ( state == CONFIGURED )
    {
        SETSOCKOPT_PARAMTYPE val = 1;
        int res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
        if(res == SOCKET_ERROR) {
            lastError = GET_NET_ERROR();
    //        doClose();
            std::stringstream msg;
            msg << "Couldn't set SO_REUSEADDR: " << NETSTRERROR(lastError);
            LOGGER.warning("%s", msg.str().c_str());
        }
    }
    else
    {
        LOGGER.warning("Trying to set reuse addr on an unconfigured socket [%s]", getStateString());
    }
}

void
SocketBase::setNoDelay()
{
#ifdef _WIN32
    if ( state >= CONFIGURED ) //  && state < LISTENING
#else
    if ( state >= CONFIGURED )
#endif

    {
        SETSOCKOPT_PARAMTYPE val = 1;
        int res = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
        if(res == SOCKET_ERROR) {
            lastError = GET_NET_ERROR();
    //        doClose();
            std::stringstream msg;
            msg << "Couldn't set TCP_NODELAY: " << NETSTRERROR(lastError);
            LOGGER.warning("%s", msg.str().c_str());
        }
    }
    else
    {
        LOGGER.warning("Trying to set nodelay on an unconfigured socket [%s]", getStateString());
    }
}

void
SocketBase::doListen()
{
    if ( state == BOUND )
    {
        int res = listen(sockfd, 20);
        if(res == SOCKET_ERROR) {
            lastError = GET_NET_ERROR();
            doClose();
            std::stringstream msg;
            msg << "Couldn't listen on socket: " << NETSTRERROR(lastError);
            throw NetworkException(msg.str());
        }
        state = LISTENING;
    }
    else
    {
        LOGGER.warning("Trying to listen on an unbound socket [%s]", getStateString());
    }
}

void
SocketBase::doConnect()
{
    if ( state == CONFIGURED )
    {
        int res = connect(sockfd, addr.getSockaddr(), addr.getSockaddrLen());
        if(res == SOCKET_ERROR)
        {
            lastError = GET_NET_ERROR();
            if ( !IS_CONNECT_INPROGRESS(lastError) )
            {
                doClose();
                std::stringstream msg;
                msg << "Couldn't connect to '" << addr.getIP() << "' port "
                    << addr.getPort() << ": " << NETSTRERROR(lastError);
                throw NetworkException(msg.str());
            }
        }
        state = CONNECTING;
        SocketManager::addSocket(this);
    }
    else
    {
        LOGGER.warning("Trying to connect to an unconfigured socket [%s]", getStateString());
    }
}

int
SocketBase::doSend(const void* data, size_t len)
{
    if ( state == CONNECTED )
    {
        int res = send(sockfd, (const char*) data, len, SEND_FLAGS);
        if(res == SOCKET_ERROR) {
            lastError = GET_NET_ERROR();
            if ( IS_IGNORABLE_ERROR(lastError) )
                return 0;

            if ( ! IS_DISCONECTED(lastError) ) {
                std::stringstream msg;
                msg << "Send error: " << NETSTRERROR(lastError);
                LOGGER.warning("%s", msg.str().c_str());
            }

            onDisconected();
            return 0;
        }
        return res;
    }
    else
    {
        //LOGGER.warning("Trying to send to unconected socket [%s]", getStateString());
        LOGGER.warning("Trying to send to unconnected socket!");
    }
    return 0;
}

int
SocketBase::doReceive(void* buffer, size_t len)
{
    if ( state == CONNECTED )
    {
        int res = recv(sockfd, (char*) buffer, len, RECV_FLAGS);
        if(res == SOCKET_ERROR) {
            lastError = GET_NET_ERROR();
            if ( IS_IGNORABLE_ERROR(lastError) )
                return 0;

            if ( ! IS_DISCONECTED(lastError) ) {
                std::stringstream msg;
                msg << "Read error: " << NETSTRERROR(lastError);
                LOGGER.warning("%s", msg.str().c_str());
            }

            onDisconected();
            return 0;
        }

        if (!res) {
            LOGGER.debug("SocketBase::doReceive Disconected from server");
            onDisconected();
        }

        return res;
    }
    else
    {
        LOGGER.warning("Trying to receive on unconected socket [%s]", getStateString());
    }
    return 0;
}

int
SocketBase::doSendTo(const Address& toaddr, const void* data, size_t len)
{
    if ( state == BOUND )
    {
        int res = sendto(sockfd, (const char*) data, len, SEND_FLAGS,
                    toaddr.getSockaddr(), toaddr.getSockaddrLen());
        if(res == SOCKET_ERROR) {
            lastError = GET_NET_ERROR();
            if ( ! IS_SENDTO_IGNORABLE(lastError) )
            {
                std::stringstream msg;
                msg << "Send error: " << NETSTRERROR(lastError);
                LOGGER.warning("%s", msg.str().c_str());
            }
            return 0;
        }
        return res;
    }
    else
    {
        LOGGER.warning("Trying to sendto in an unbound socket [%s]", getStateString());
    }
    return 0;
}

size_t
SocketBase::doReceiveFrom(Address& fromaddr, void* buffer, size_t len)
{
    if ( state == BOUND || state == CONNECTED )
    {
        int res = recvfrom(sockfd, (char*) buffer, len, RECV_FLAGS,
                fromaddr.getSockaddr(), fromaddr.getSockaddrLenPointer());
        if ( res == SOCKET_ERROR )
        {
            lastError = GET_NET_ERROR();
            if ( ! IS_RECVFROM_IGNORABLE(lastError) )
            {
                std::stringstream msg;
                msg << "ReceiveFrom error: " << NETSTRERROR(lastError);
                LOGGER.warning("%s", msg.str().c_str());
            }

            return 0;
        }
        return res;
    }
    else
    {
        LOGGER.warning("Trying to receivefrom on an not bound or conected socket [%s]", getStateString());
    }
    return 0;
}

SOCKET
SocketBase::doAccept(Address& fromaddr)
{
    if ( state == LISTENING )
    {
        SOCKET newsock;
        newsock= accept(sockfd, fromaddr.getSockaddr(), fromaddr.getSockaddrLenPointer());
        if ( newsock == INVALID_SOCKET )
        {
            lastError = GET_NET_ERROR();
            if ( ! IS_ACCEPT_IGNORABLE(lastError) )
            {
                std::stringstream msg;
                msg << "Accept error: " << NETSTRERROR(lastError);
                LOGGER.warning("%s", msg.str().c_str());
            }
        }
        return newsock;
    }
    else
    {
        LOGGER.warning("Trying to accept on an unlistening socket [%s]", getStateString());
    }
    return SOCKET_ERROR;
}

void
SocketBase::doClose()
{
    LOGGER.debug("SocketBase:: Closing [%d] socket", sockfd);
    SocketManager::removeSocket(this);
}

}

