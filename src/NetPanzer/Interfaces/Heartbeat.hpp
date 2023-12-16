/*
Copyright (C) 2007 by Aaron Perez <aaronps@gmail.com>

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

#ifndef _HEARTBEAT_HPP
#define _HEARTBEAT_HPP

#include "Network/TCPSocket.hpp"
#include "Util/NTimer.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <map>

using namespace network;

class MasterserverInfo;

class Heartbeat : public TCPSocketObserver
{
public:
    Heartbeat();
    ~Heartbeat();
    void checkHeartbeat();
    
protected:
    void onConnected(TCPSocket *so);
    void onDisconnected(network::TCPSocket *so);
    void onDataReceived(TCPSocket *so, const char *data, const int len);
    void onSocketError(TCPSocket *so);
    void startHeartbeat();
    
private:
    NTimer nextHeartbeat;
    std::string hb_message;
    std::vector<Address> mslist;
    typedef std::map<TCPSocket *, MasterserverInfo *> MSMap;
    typedef MSMap::iterator MSMapIterator;
    MSMap masterservers;
};

#endif
