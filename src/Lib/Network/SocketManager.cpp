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

#include <sstream>
#include "SocketManager.hpp"
#include "NetworkManager.hpp"
#include "Util/Log.hpp"

#include <cstdio>
#include <cstring>
#include <errno.h>

namespace network {

SocketManager::Sockets SocketManager::socketList;
SocketManager::Sockets SocketManager::newSockets;
SocketSet SocketManager::sset;

void
SocketManager::handleEvents()
{
    SocketsIterator i;

    if (!newSockets.empty()) {
        for (i = newSockets.begin(); i!=newSockets.end(); i++) {
            LOGGER.debug("SocketManager:: Adding socket [%d,%08lx]",(*i)->sockfd, (unsigned long)(*i));
            socketList.insert(*i);
        }
        newSockets.clear();
    }

    // Make the socketset every time, not all OS works the same way
    sset.clear();
    SocketBase *sb = 0;
    for ( SocketsIterator i = socketList.begin(), i_next = i, i_end = socketList.end();
          i != i_end; i = i_next)
    {
        ++i_next;
        sb = (*i);
        switch ( sb->state )
        {
            case SocketBase::RESOLVING :
                switch ( NetworkManager::queryAddress(sb->addr)  )
                {
                    case Address::ST_OK :
                        sb->onResolved();
                        break;

                    case Address::ST_ERROR :
                        sb->onSocketError();
                        break;
                }
                break;

            case SocketBase::CONNECTING :
                sset.addWrite(sb);
                break;

            case SocketBase::BOUND:
            case SocketBase::LISTENING :
            case SocketBase::CONNECTED :
                sset.add(*i);
                break;

            case SocketBase::DESTROYING :
                if ( sb->disconnectTimer.isTimeOut() )
                {
                    LOGGER.debug("SocketManager:: Removing socket [%d,%08lx]",sb->sockfd, (unsigned long)sb);
                    delete sb;
                    socketList.erase(i);
                }
                break;
        }
    }

    int r = sset.select(0);
    if ( !r ) // most common first
        return;
    
    if ( r > 0 )
    {
        for ( SocketsIterator i = socketList.begin(), i_next = i, i_end = socketList.end();
              i != i_end; i = i_next)
        {
            ++i_next;
            sb = (*i);
            switch ( sb->state )
            {
                case SocketBase::CONNECTING :
                    if ( sset.isWriteable(sb) )
                    {
                        sb->connectionFinished();
                    }
                    break;

                case SocketBase::BOUND:
                case SocketBase::LISTENING :
                case SocketBase::CONNECTED :
                    if ( sset.dataAvailable(sb) )
                    {
                        sb->onDataReady();
                    }
                    break;

                case SocketBase::DESTROYING :
                    if ( sb->disconnectTimer.isTimeOut() )
                    {
                        LOGGER.debug("SocketManager:: Removing socket2 [%d,%08lx]",sb->sockfd, (unsigned long)sb);
                        delete sb;
                        socketList.erase(i);
                    }
                    break;
            }
        }
    }
    else  // some error happened
    {
        int error = sset.getError();
        if ( IS_INVALID_SOCKET(error) )
        {
            removeInvalidSockets();
        }
        else if ( !IS_INTERRUPTED(error) ) // beware: is NOT interrupted
        {
            std::stringstream msg;
            msg << NETSTRERROR(error);
            LOGGER.debug("SocketManager: BAD BAD ERROR %s", msg.str().c_str());
        }
    }
}

void
SocketManager::removeInvalidSockets()
{
    LOGGER.debug("Finding invalid sockets in the set...");
    SocketBase *sb;

    for ( SocketsIterator i = socketList.begin(), i_next = i, i_end = socketList.end();
          i != i_end; i = i_next)
    {
        ++i_next;
        sb = (*i);
        sset.clear();
        switch ( sb->state )
        {
            case SocketBase::CONNECTING :
                if ( sset.isWriteable(sb) )
                {
                    sb->connectionFinished();
                }
                break;

            case SocketBase::BOUND:
            case SocketBase::LISTENING :
            case SocketBase::CONNECTED :
                if ( sset.dataAvailable(sb) )
                {
                    sb->onDataReady();
                }
                break;
            default:
                continue;
        }

        if ( sset.select() < 0 )
        {
            int error = GET_NET_ERROR();
            if ( IS_INVALID_SOCKET(error) )
            {
                LOGGER.warning("SocketManager: FOUND Invalid socket, removing...");
                sb->onSocketError();
                removeSocket(sb);
            }
            else
            {
                std::stringstream msg;
                msg << NETSTRERROR(error);
                LOGGER.debug("SocketManager: Error while finding invalid sockets %s", msg.str().c_str());
            }
        }
    }

}

}
