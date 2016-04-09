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
#ifndef __TCPSOCKET_HPP__
#define __TCPSOCKET_HPP__

#include "SocketBase.hpp"
#include "Address.hpp"

#include <string>

namespace network
{

class TCPSocket;    
    
class TCPSocketObserver {
public:    
    TCPSocketObserver() {};
    virtual ~TCPSocketObserver() {};
protected:
    friend class TCPSocket;
    virtual void onDataReceived(TCPSocket *so, const char *data, const int len) = 0;
    virtual void onConnected(TCPSocket *so) = 0;
    virtual void onDisconected(TCPSocket *so) = 0;
    virtual void onSocketError(TCPSocket *so) = 0;
};

class TCPSocket : public SocketBase
{
public:
//    TCPSocket(const Address& bindaddr, const Address& address, bool blocking = true);


    TCPSocket(const Address& address, TCPSocketObserver *o) throw(NetworkException);
    TCPSocket(const std::string& host,const std::string& port, TCPSocketObserver *o) throw(NetworkException);

    void destroy();

    size_t send(const void* data, size_t datasize) throw(NetworkException);
    
protected:
    ~TCPSocket();
    void onDataReady();
    void onConnected();
    void onDisconected();
    void onSocketError();
    void onResolved();
    
private:
    friend class TCPListenSocket;

    TCPSocket(SOCKET fd, const Address& addr, TCPSocketObserver *o) throw(NetworkException);

    TCPSocketObserver *observer;
};

}

#endif

