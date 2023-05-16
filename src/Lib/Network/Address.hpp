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
#ifndef __ADDRESS_HPP__
#define __ADDRESS_HPP__

#include "SocketHeaders.hpp"
#include "NetworkException.hpp"
#include <string>

namespace network
{

class Address
{
public:

    enum A_STATUS
    {
        ST_ERROR,
        ST_UNRESOLVED,
        ST_WAITING,
        ST_RESOLVING,
        ST_OK
    };

    Address(bool isTcp = true, bool forBinding = true);
    Address(const Address& other);

    void setParams(const std::string& host, const std::string& port);

    static Address resolve(const std::string& name, unsigned short port, bool isTcp = true, bool forBinding = false);

    std::string getIP() const;

    /** returns the port of this address */
    unsigned short getPort() const;
    void setPort(unsigned short port);

    void operator=(const Address& other);
    bool operator==(const Address& other) const;

    bool compareDest(const Address& other) const
    {
        return str_host == other.str_host && str_port == other.str_port;
    }

    struct sockaddr * getSockaddr() const { return (struct sockaddr *)&ss; };
    socklen_t getSockaddrLen() const
    {
#ifdef __APPLE__
        return sizeof(sockaddr_in); 
#else
        return ss_len;
#endif
    };
    socklen_t * getSockaddrLenPointer() { return &ss_len; };
    
    static Address ANY;

private:
    friend class NetworkManager;
    friend class SocketBase;
    friend class SocketManager;

    enum A_STATUS status;
    std::string str_host;
    std::string str_port;
    int socktype;
    int protocol;
    bool forBinding;
    socklen_t ss_len;
    struct sockaddr_storage ss;
};

}

#endif

