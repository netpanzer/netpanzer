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
#ifndef __SERVERQUERYTHREAD_HPP__
#define __SERVERQUERYTHREAD_HPP__

#include <vector>
#include <string>
#include <map>
#include <SDL2/SDL_thread.h>
#include "Network/TCPSocket.hpp"
#include "Network/UDPSocket.hpp"
#include "Network/Address.hpp"

#include "ServerList.hpp"

namespace masterserver
{
    
class MSInfo;

class ServerQueryThread : public network::TCPSocketObserver, public network::UDPSocketObserver
{
public:
    ServerQueryThread(ServerList* serverlist);
    ~ServerQueryThread();

    const char* getStateMessage() const;
    bool isRunning() const
    {
        return running;
    }
    void checkTimeOuts();
    
protected:
    void onDataReceived(network::TCPSocket *s, const char *data, const int len);
    void onConnected(network::TCPSocket *s);
    void onDisconnected(network::TCPSocket *s);
    void onDataReceived(network::UDPSocket *s, const network::Address &from, const char *data, const int len);
    void onSocketError(network::TCPSocket *s);
    void onSocketError(network::UDPSocket *s);
    
private:
    
    void parseServerData(ServerInfo *server, std::string &data);
    void sendNextQuery();
    void sendQuery(ServerInfo *server);

    void queryMasterServer();
    void queryServers();

    bool running;
    
    ServerList* serverlist;

    enum State {
        STATE_NOSERVERS,
        STATE_QUERYMASTERSERVER,
        STATE_QUERYSERVERS,
        STATE_ERROR,
        STATE_DONE
    };
    State state;

    std::vector<std::string> masterservers;

    network::UDPSocket* udpsocket;

    std::vector<ServerInfo*> not_queried;
    int queries; // number of currently running queries
    
    std::map<network::TCPSocket *,MSInfo *> querying_msdata;
    std::map<std::string, ServerInfo *> querying_server;
};

} // masterserver

#endif

