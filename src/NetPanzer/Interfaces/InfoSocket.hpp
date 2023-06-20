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

#ifndef _INFOSOCKET_HPP
#define _INFOSOCKET_HPP

#include "Network/UDPSocket.hpp"
#include <string>

using namespace network;

class InfoSocket : public UDPSocketObserver
{
public:
    InfoSocket(int p);
    ~InfoSocket();

protected:
    void onDataReceived(UDPSocket *so, const Address &from, const char *data, const int len);
    void onSocketError(UDPSocket *so);
    
private:
    std::string prepareStatusPacket();
    std::string prepareFlagPacket(const int flagNum);
    std::string statusHead;
    UDPSocket * socket;
    
};

#endif
