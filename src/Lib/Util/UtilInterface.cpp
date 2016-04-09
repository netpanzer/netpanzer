/*
Copyright (C) 1998 Pyrosoft Inc. (www.pyrosoftgames.com), Matthew Bogue
 
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

#include <sstream>

#include <sys/stat.h>
#include <string.h>
#include "Exception.hpp"
#include "FileSystem.hpp"
#include "UtilInterface.hpp"
#include "Log.hpp"

// split server:port string, doesn't always set the port
void UtilInterface::splitServerPort(const std::string& server,std::string& address,int *port)
{
    std::string::size_type colon = server.find(':',0);
    if(colon == std::string::npos) {
        address=server;
    } else {
        address=server.substr(0, colon);
        colon++;
        std::string port_str(server.substr(colon, server.length() - colon));
        port[0]=atoi(port_str.c_str());
    }
}

