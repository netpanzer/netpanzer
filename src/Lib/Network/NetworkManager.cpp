/*
Copyright (C) 2012 by Aaron Perez <aaronps@gmail.com>

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

#include "NetworkManager.hpp"
#include "SocketHeaders.hpp"
#include "SocketManager.hpp"
#include "Util/Log.hpp"

#include <list>

#include <map>
#include <SDL2/SDL.h>

namespace network
{

static bool network_running = false;

static SDL_Thread * resolverThread = 0;
static SDL_sem * semaphore = 0;

static Address thread_work;
static int to_resolve = 0;

static bool resolving = false;
static std::list<Address> rqueue;

static bool resolver_busy = false;
int NetworkManager::resolver_worker(void *data)
{
    (void)data;
    while ( network_running )
    {
        resolver_busy = false;
        SDL_SemWait(semaphore);
        if ( ! network_running ) break;

        struct addrinfo hints;
        struct addrinfo *firstaddress = 0;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = thread_work.socktype;
        hints.ai_protocol = thread_work.protocol;
        if ( thread_work.forBinding )
        {
            hints.ai_flags = AI_PASSIVE;
        }

        hints.ai_flags |= AI_NUMERICSERV;

        int res = getaddrinfo(thread_work.str_host.size() > 0 ? thread_work.str_host.c_str() : 0,
                              thread_work.str_port.c_str(),
                              &hints, &firstaddress);
        if ( res )
        {
            freeaddrinfo(firstaddress);
            thread_work.status = Address::ST_ERROR;
        }
        else
        {
            thread_work.ss_len = firstaddress->ai_addrlen;
            memcpy(&thread_work.ss, firstaddress->ai_addr, firstaddress->ai_addrlen);
            freeaddrinfo(firstaddress);

            thread_work.status = Address::ST_OK;
        }
    }
    return 0;
}


bool NetworkManager::initialize()
{
#ifdef _WIN32
    WSADATA wsaData;
    WORD wVers = MAKEWORD(2, 0);
    int rc = WSAStartup(wVers, &wsaData);
    if(rc != 0) {
        fprintf(stderr, "Failed to initialize winsock: %d\n", rc);
        return false;
    }
#endif

    network_running = true;
    semaphore = SDL_CreateSemaphore(0);
    resolverThread = SDL_CreateThread(resolver_worker, "np-resolver", 0);

    return true;
}

void NetworkManager::cleanUp()
{
    network_running = false;

    SDL_SemPost(semaphore);
    int st;
    SDL_WaitThread(resolverThread, &st);

    SDL_DestroySemaphore(semaphore);
    semaphore = 0;
    resolverThread = 0;

    rqueue.clear();

#ifdef _WIN32
    WSACleanup();
#endif
}

void NetworkManager::run()
{
    if ( resolving && ! resolver_busy ) // finished resolv
    {
        resolving = false;
        bool found_wanted = false;
        for ( std::list<Address>::iterator i = rqueue.begin(); i != rqueue.end(); i++ )
        {
            if ( thread_work.compareDest(*i) )
            {
                (*i) = thread_work;
                found_wanted = true;
                break;
            }
        }

        --to_resolve;
        resolving = false;

        if ( ! found_wanted )
        {
            LOGGER.warning("Resolved something unwanted");
        }
    }

    if ( to_resolve > 0 && ! resolving )
    {
        bool found_waiting = false;
        for ( std::list<Address>::iterator i = rqueue.begin(); i != rqueue.end(); i++ )
        {
            if ( (*i).status == Address::ST_WAITING )
            {
                (*i).status = Address::ST_RESOLVING;
                thread_work = (*i);
                found_waiting = true;
                break;
            }
        }

        resolver_busy = true;
        resolving = true;
        SDL_SemPost(semaphore);

        if ( ! found_waiting )
        {
            LOGGER.warning("to_resolve has something but there is no waiting here");
        }
    }

    SocketManager::handleEvents();
}

int NetworkManager::queryAddress(Address &address)
{
    struct addrinfo hints;
    struct addrinfo *firstaddress = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = address.socktype;
    hints.ai_protocol = address.protocol;
    if ( address.forBinding )
    {
        hints.ai_flags = AI_PASSIVE;
    }

    hints.ai_flags |= AI_NUMERICHOST|AI_NUMERICSERV;

    int res = getaddrinfo(address.str_host.size() > 0 ? address.str_host.c_str() : 0,
                          address.str_port.c_str(),
                          &hints, &firstaddress);
    if ( res )
    {
        freeaddrinfo(firstaddress);

        for ( std::list<Address>::iterator i = rqueue.begin(); i != rqueue.end(); i++ )
        {
            if ( address.compareDest(*i) )
            {
                switch ( (*i).status )
                {
                    case Address::ST_OK :
                        address.ss_len = (*i).ss_len;
                        memcpy(&address.ss, &(*i).ss, (*i).ss_len);
                        // fall through

                    case Address::ST_ERROR :
                        address.status = (*i).status;
                        rqueue.erase(i);
                        break;

                    default:
                        address.status = (*i).status;

                }

                return address.status;
            }
        }
        address.status = Address::ST_WAITING;
        rqueue.push_back(address);
        ++to_resolve;
    }
    else
    {
        address.ss_len = firstaddress->ai_addrlen;
        memcpy(&address.ss, firstaddress->ai_addr, firstaddress->ai_addrlen);
        freeaddrinfo(firstaddress);

        address.status = Address::ST_OK;
    }

    return address.status;
}

} // namespace
