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

#ifndef _SOCKETMANAGER_HPP
#define _SOCKETMANAGER_HPP

#include <set>
#include "SocketBase.hpp"
#include "TCPSocket.hpp"
#include "SocketSet.hpp"

namespace network {
    
class SocketManager
{
public:
    SocketManager() {};
    ~SocketManager() {};
    
    static void handleEvents();
    
protected:
    friend class SocketBase; // the only allowed to add/remove
    static void addSocket(SocketBase *s)
    {
        newSockets.insert(s);
    }
    
    static void removeSocket(SocketBase *s)
    {
        s->state = SocketBase::DESTROYING;
    }
    
    static void removeInvalidSockets();

private:
    typedef std::set<SocketBase *> Sockets;
    typedef std::set<SocketBase *>::iterator SocketsIterator;
    
    static SocketSet sset;
    static Sockets socketList;
    static Sockets newSockets;
};

} // namespace

#endif
