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
#ifndef __SERVERINFO_HPP__
#define __SERVERINFO_HPP__

#include <string>

#include <SDL2/SDL.h>
#include "Network/Address.hpp"

namespace masterserver
{

/**
 * This class contains informations about running gameservers
 */
class ServerInfo
{
public:
    enum Status {
        RUNNING,
        QUERYING,
        TIMEOUT
    };

    ServerInfo();
    ~ServerInfo();

    std::string name;
    std::string address;
    int port;

    Status status;
    int players;
    int maxplayers;
    std::string map;
    int ping;
    int protocol;
    bool needs_password;
    bool auth_on;

    network::Address ipaddress;
    Uint32 querystartticks;

    int tryNum;
};

} // masterserver

#endif

