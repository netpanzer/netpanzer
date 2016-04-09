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


#include "TCPListenSocket.hpp"
#include "Util/Log.hpp"

namespace network
{
    
TCPListenSocket::TCPListenSocket(const Address& newaddr, TCPListenSocketObserver *o)
    : SocketBase(newaddr,true), observer(o)
{
    setReuseAddr();
    bindSocket();
    doListen();
}

void
TCPListenSocket::destroy()
{
    doClose();
}

void
TCPListenSocket::onSocketError()
{
    if ( observer )
        observer->onSocketError(this);
}

void
TCPListenSocket::onDataReady()
{
    Address newaddr;
    SOCKET newsock;
    TCPSocketObserver * newobserver;
    try {
        while ( (newsock=doAccept(newaddr)) != INVALID_SOCKET) {
            newobserver = observer->onNewConnection(this, newaddr);
            TCPSocket * newcon = new TCPSocket(newsock,newaddr,newobserver);
            newcon->setNoDelay();
            newcon->onConnected();
        }
    } catch (NetworkException e) {
        LOGGER.warning("Error Accepting new connections: '%s'", e.what());
    }
}


}
