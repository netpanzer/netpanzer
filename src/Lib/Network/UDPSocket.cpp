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



#include "UDPSocket.hpp"
#include <sstream>
#include "Util/Log.hpp"

namespace network
{

UDPSocket::UDPSocket(UDPSocketObserver *o)
    : SocketBase(Address::ANY, false), observer(o)
{
    bindSocket();
}

UDPSocket::UDPSocket(const Address& bindaddr, UDPSocketObserver *o)
    : SocketBase(bindaddr,false), observer(o)
{
    bindSocket();
}

UDPSocket::~UDPSocket()
{
}

void
UDPSocket::destroy()
{
    observer=0;
    doClose();
}

void
UDPSocket::onSocketError()
{
    if ( observer )
        observer->onSocketError(this);
}

void
UDPSocket::send(const Address& toaddr, const void* data, size_t datasize)
{
    int res = doSendTo(toaddr,data,datasize);
    if(res != (int) datasize)
    {
        LOGGER.warning("Send error: not all data sent.");
    }
}

void
UDPSocket::onDataReady()
{
    Address a;
    char buffer[4096];
    int len;
    do {
        len=doReceiveFrom(a,buffer,sizeof(buffer));
        if (len && observer)
            observer->onDataReceived(this,a,buffer,len);
    } while (len && observer);
}

}
