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
#ifndef __SOCKETSET_HPP__
#define __SOCKETSET_HPP__

#include "SocketHeaders.hpp"
#include "SocketBase.hpp"
#include "Util/NoCopy.hpp"

namespace network
{

class SocketSet : public NoCopy
{
public:
    SocketSet()
    {
        clear();
    }
    
    ~SocketSet(){};

    inline void clear()
    {
        FD_ZERO(&readset);
        FD_ZERO(&writeset);
        maxfd        = 0;
        readset_ptr  = 0;
        writeset_ptr = 0;
    }
    
    /** add a socket to the set that should be watched */
    inline void add(const SocketBase* socket)
    {
        FD_SET(socket->sockfd, &readset);
        SETMAXFD(maxfd,socket->sockfd);
        readset_ptr = &readset;
    }

    inline void addWrite(const SocketBase* socket)
    {
        FD_SET(socket->sockfd, &writeset);
        SETMAXFD(maxfd,socket->sockfd);
        writeset_ptr = &writeset;
    }

    /** Waits for input on the sockets in the set until a socket has input or a
     * timeout occurs. Returns false in case of timeout.
     */
    int select(unsigned int timeout_usec = 0)
    {
        if ( !readset_ptr && !writeset_ptr )
            return 0;

        timeout.tv_sec = timeout_usec/1000000;
        timeout.tv_usec = timeout_usec%1000000;
        
        int res = ::select(maxfd+1, readset_ptr, writeset_ptr, 0, &timeout);
        if ( res == SOCKET_ERROR ) {
            select_error = GET_NET_ERROR();
            return -1;
        }

        return res;
    }

    /** after a select call you can test with this function if a specific socket
     * has data pending
     */
    
    inline bool dataAvailable(const SocketBase* socketbase) const
    {
        return FD_ISSET(socketbase->sockfd, (fd_set *)&readset);
    }
    
    inline bool isWriteable(const SocketBase* socketbase) const
    {
        return FD_ISSET(socketbase->sockfd, (fd_set *)&writeset);
    }
    
    inline int getError() const { return select_error; }
    
private:
    SOCKET maxfd;
    fd_set readset;
    fd_set writeset;
    fd_set *readset_ptr;
    fd_set *writeset_ptr;
    struct timeval timeout;
    int select_error;
};

}

#endif

