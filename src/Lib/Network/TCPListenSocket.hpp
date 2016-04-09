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
#ifndef __TCPLISTENSOCKET_HPP__
#define __TCPLISTENSOCKET_HPP__

#include "SocketBase.hpp"
#include "TCPSocket.hpp"
#include "NetworkException.hpp"

namespace network
{
    
class TCPListenSocket;    
    
class TCPListenSocketObserver {
public:    
    TCPListenSocketObserver() {};
    virtual ~TCPListenSocketObserver() {};
protected:
    friend class TCPListenSocket;
    virtual TCPSocketObserver * onNewConnection(TCPListenSocket *so, const Address &fromaddr) = 0;
    virtual void onSocketError(TCPListenSocket *so) = 0;
};
    

class TCPListenSocket : public SocketBase
{
public:
    TCPListenSocket(const Address& bindaddr, TCPListenSocketObserver *o);
    void destroy();
    
protected:
    ~TCPListenSocket() {};
    
    void onDataReady();
    void onSocketError();

private:
    TCPListenSocketObserver * observer;
    
};

}

#endif

