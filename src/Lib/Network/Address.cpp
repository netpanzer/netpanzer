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


#include <sstream>
#include <string>
#include <string.h>

#include "Address.hpp"
#include "NetworkException.hpp"

#include "Util/Log.hpp"

namespace network
{

Address Address::ANY(false, true);

Address::Address(bool isTcp, bool forBinding)
{
    memset(&ss, 0, sizeof(ss));
    status = ST_OK;
    socktype = isTcp ? SOCK_STREAM : SOCK_DGRAM;
    protocol = isTcp ? IPPROTO_TCP : IPPROTO_UDP;
    this->forBinding = forBinding;
    ss_len=sizeof(ss);
    ss.ss_family = AF_INET;
    //addr.sin_addr.s_addr = INADDR_ANY;
}

Address::Address(const Address& other)
{
    memcpy(&ss, &other.ss, sizeof(ss));
    ss_len = other.ss_len;
    str_host = other.str_host;
    str_port = other.str_port;
    status = other.status;
    socktype = other.socktype;
    protocol = other.protocol;
    forBinding = other.forBinding;
}

void
Address::setParams(const std::string &host, const std::string &port)
{
    str_host = host;
    str_port = port;
    status = ST_UNRESOLVED;
}

void
Address::operator=(const Address& other)
{
    if(&other == this) // ignore self assignment
        return;
    memcpy(&ss, &other.ss, sizeof(ss));
    ss_len=other.ss_len;
    str_host = other.str_host;
    str_port = other.str_port;
    status = other.status;
    socktype = other.socktype;
    protocol = other.protocol;
    forBinding = other.forBinding;
}

bool
Address::operator==(const Address& other) const
{
    return !memcmp(&ss,&other.ss,ss_len); // todo: check for ss_len too
}

std::string
Address::getIP() const
{
    // XXX quick hack to get it working until get full ss support
    // TODO: make a real ipaddress conversion function
    return std::string(inet_ntoa(((struct sockaddr_in &)ss).sin_addr));
}

unsigned short
Address::getPort() const
{
    // XXX quick hack to get it working until get full ss support
    // TODO: make a real port conversion function
    return ntohs(((struct sockaddr_in &)ss).sin_port);
}

void
Address::setPort(unsigned short port)
{
    ((struct sockaddr_in &)ss).sin_port = htons(port);
}

Address
Address::resolve(const std::string& name, unsigned short port, bool isTcp, bool forBinding)
{
    LOGGER.debug("Address:: Resolving '%s':%u", name.c_str(), port);
    Address result(isTcp, forBinding);

    struct addrinfo hints;
    struct addrinfo *firstaddress = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    hints.ai_socktype = result.socktype;
    hints.ai_protocol = result.protocol;
    if ( result.forBinding )
    {
        hints.ai_flags = AI_PASSIVE;
    }

    std::stringstream po;
    po << port;

    int res = getaddrinfo(name.size() > 0 ? name.c_str() : 0, po.str().c_str(), &hints, &firstaddress);
    if ( res )
    {
        std::stringstream msg;
        msg << "Couldn't resolve address '" << name;
        if (res == EAI_NONAME)
        {
             msg << "' Name not found";
        }
        else
        {
            msg << "' some other error";
        }

        if ( firstaddress ) freeaddrinfo(firstaddress);

        throw NetworkException(msg.str());
    }

    result.ss_len = firstaddress->ai_addrlen;
    memcpy(&result.ss, firstaddress->ai_addr, firstaddress->ai_addrlen);

    freeaddrinfo(firstaddress);

    return result;

#if 0
    Address result;

    result.setPort(port);
    
    if(name == "")
    {
        return result;
    }

    struct hostent* hentry = gethostbyname(name.c_str());
    if(!hentry) {
        std::stringstream msg;
        msg << "Couldn't resolve address '" << name;
#ifdef _WIN32
        msg << "' (code " << WSAGetLastError() << ")";
#else
        msg << "': " << hstrerror(h_errno);
#endif
        throw NetworkException(msg.str());

    }

    ((struct sockaddr_in &)result.ss).sin_addr.s_addr = ((struct in_addr*) hentry->h_addr)->s_addr;

    return result;
#endif
}

}
