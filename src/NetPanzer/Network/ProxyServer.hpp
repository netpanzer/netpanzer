/*
Copyright (C) 2003 by Matthias Braun <matze@braunis.de>

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
#ifndef __PROXYSERVER_HPP__
#define __PROXYSERVER_HPP__

#include <string>
#include <sstream>

#include "Network/TCPSocket.hpp"

class ProxyServer
{
public:
    std::string proxyserver;
    std::string proxyserveruser;
    std::string proxyserverpass;

    void setProxy(const std::string& p,const std::string& u="",const std::string& pass="") {
        proxyserver=p; proxyserveruser=u; proxyserverpass=pass; 
    }
    static void makeBase64(std::string& base64, const std::string& str);
    void getProxyConnect(std::stringstream& buffer,
            const std::string& serveraddress);
    void sendProxyConnect(network::TCPSocket& socket,
            const std::string& serveraddress);
};

#endif

