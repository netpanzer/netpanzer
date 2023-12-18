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
#ifndef __SOCKETBASE_HPP__
#define __SOCKETBASE_HPP__

#include <iostream>
#include <stdexcept>
#include "NetworkException.hpp"
#include "SocketHeaders.hpp"
#include "Address.hpp"
#include "Util/NoCopy.hpp"
#include "Util/NTimer.hpp"

namespace network
{

/** Base class for sockets, this is intended for internal use only */
class SocketBase : public NoCopy
{
public:
    const Address& getAddress() const
    {
        return addr;
    }

protected:
    friend class SocketSet;
    friend class SocketManager;

    virtual ~SocketBase();
    
    SocketBase();
    SocketBase(const Address &a, bool isTcp);
    SocketBase(SOCKET fd, const Address &a);
        
    virtual void onDataReady() = 0;
    virtual void onDisconected() {}
    virtual void onConnected() { state = CONNECTED; }
    virtual void onResolved() { state = RESOLVED; }
    virtual void onSocketError() = 0;
    virtual void destroy() = 0;
    
    void setAddress(const Address &a);

    void setReuseAddr();
    void setNoDelay();
    void doClose();
    
    void bindSocketTo(const Address& toaddr);
    void bindSocket() { bindSocketTo(addr); };
    void doListen();
    void doConnect();
    int  doSend(const void* data, size_t len);
    int  doReceive(void* buffer, size_t len);
    int  doSendTo(const Address& toaddr, const void* data, size_t len);
    size_t  doReceiveFrom(Address& fromaddr, void* buffer, size_t len);
    SOCKET doAccept(Address& fromaddr);
    
    void setConfigured() { state = CONFIGURED; }

    void create();
    void setNonBlocking();
private:

    enum {
        ST_ERROR,
        UNINITIALIZED,
        RESOLVING,
        RESOLVED,
        CREATED,
        CONFIGURED,
        BOUND,
        LISTENING,
        CONNECTING,
        CONNECTED,
        DESTROYING
    };

    static const char *state_str[];
    const char * getStateString() { return state_str[state]; }

    
    void connectionFinished()
    {
        onConnected();
    };
    
    int state;
    SOCKET sockfd;
    Address addr;
    int lastError;



    NTimer disconnectTimer;
};

}

#endif

