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


#include "ServerQueryThread.hpp"

#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <ctype.h>
#include "Util/Exception.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Core/NetworkGlobals.hpp"
#include "Util/StringTokenizer.hpp"
#include "Util/StreamTokenizer.hpp"
#include "Util/Log.hpp"
#include "Util/StringUtil.hpp"

static const size_t MAX_QUERIES = 4;
static const Uint32 QUERY_TIMEOUT = 3 * 1000;
static const Uint32 MS_TIMEOUT = 12 * 1000;

namespace masterserver
{

class MSInfo {
public:
    MSInfo() { touch(); };
    void touch() { lastTicks = SDL_GetTicks(); };
    std::string recdata;
    Uint32 lastTicks;
};


ServerQueryThread::ServerQueryThread(ServerList* newserverlist)
    : running(true),
      serverlist(newserverlist), state(STATE_QUERYMASTERSERVER), udpsocket(0),
      queries(0)
{
    // parse masterserverlist
    std::string server;
    StringTokenizer tokenizer(*GameConfig::server_masterservers, ',');
    while( !(server = tokenizer.getNextToken()).empty()) {
        masterservers.push_back(removeSurroundingSpaces(server));
    }
    //std::random_shuffle(masterservers.begin(), masterservers.end());

    if(masterservers.empty()) {
        state = STATE_NOSERVERS;
        running = false;
        return;
    }
    try {
        udpsocket = new network::UDPSocket(this);
        queryMasterServer();
    } catch (...) {
        running = false;
        if (udpsocket)
            udpsocket->destroy();
    }
}

ServerQueryThread::~ServerQueryThread()
{
    if (udpsocket)
        udpsocket->destroy();

    if ( ! querying_msdata.empty() ) {
        std::map<network::TCPSocket *,MSInfo *>::iterator msiter;
        for (msiter = querying_msdata.begin(); msiter != querying_msdata.end(); msiter++) {
            delete msiter->second;
            msiter->first->destroy();
        }
        querying_msdata.clear();
    }

    if ( ! querying_server.empty() ) {
        querying_server.clear();
    }

}

void
ServerQueryThread::queryMasterServer()
{
    while (!masterservers.empty()) {
        try {
            std::string masterserverip = masterservers.back();
            masterservers.pop_back();

//            network::Address ip
//                = network::Address::resolve(masterserverip, 28900);

//            network::TCPSocket *s = new network::TCPSocket(ip, this);
            LOGGER.info("Querying masterserver: %s.", masterserverip.c_str());
            network::TCPSocket *s = new network::TCPSocket(masterserverip, "28900", this);
            MSInfo * msi = new MSInfo();
            querying_msdata[s]=msi;
            running = true;
        } catch(std::exception& e) {
            LOGGER.warning("Problem querying masterserver: %s.", e.what());
        }
    }
}

void
ServerQueryThread::onConnected(network::TCPSocket *s)
{
    LOGGER.info("MASTERSERVER Connected [%s], sending query.", s->getAddress().getIP().c_str());
//    char query[] = "\\list\\gamename\\master\\final\\list\\gamename\\netpanzer\\final\\";
    char query[] = "\\list\\gamename\\netpanzer\\final\\";

    querying_msdata[s]->touch();
    s->send(query,sizeof(query)-1);
}

void
ServerQueryThread::onDisconnected(network::TCPSocket *s)
{
    LOGGER.warning("MASTERSERVER Disconnected [%s]", s->getAddress().getIP().c_str());
    delete querying_msdata[s];
    querying_msdata.erase(s);
}

void
ServerQueryThread::onSocketError(network::TCPSocket *s)
{
    LOGGER.warning("MASTERSERVER Socket error [%s]", s->getAddress().getIP().c_str());
    delete querying_msdata[s];
    querying_msdata.erase(s);
}

void
ServerQueryThread::onSocketError(network::UDPSocket *s)
{
    LOGGER.warning("SERVER query socket error [%s]", s->getAddress().getIP().c_str());
    udpsocket = nullptr;
}

void
ServerQueryThread::onDataReceived(network::TCPSocket *s, const char *data, const int len)
{
    LOGGER.debug("ServerQueryThread: Received [%s] from server [%s]", data, s->getAddress().getIP().c_str());

    std::string str;

    MSInfo * msi = querying_msdata[s];
    msi->touch();
    str = msi->recdata;
    str.append(data,len);

    if (str[0] != '\\') {
        delete msi;
        querying_msdata.erase(s);
        s->destroy();
        return; // invalid answer;
    }

    std::string lastpart;
    if (str[str.length()-1] != '\\') {
        // received incomplete
        std::string::size_type p = str.rfind('\\');
        msi->recdata = str.substr(p);
        str.erase(p);
    } else {
        msi->recdata = "\\";
    }

    StringTokenizer tknizer(str,'\\');

    std::string token = tknizer.getNextToken();
    while ( !token.empty()) {
        if ( token == "ip" ) {
            std::string dirip = tknizer.getNextToken();
            std::string port;
            if ( dirip.empty() ) {
                msi->recdata.insert(0,"\\ip\\");
                break;
            }

            token = tknizer.getNextToken();
            if ( token.empty() ) {
                msi->recdata.insert(0,dirip.insert(0,"\\ip\\")+"\\");
                break;
            }

            if ( token == "port" ) {
                token = tknizer.getNextToken();
                if (token.empty()) {
                    msi->recdata.insert(0,dirip.insert(0,"\\ip\\")+"\\port\\");
                    break;
                }
                port=token;
                token = tknizer.getNextToken();
            }

            LOGGER.warning("Server IP received: [%s:%s]",dirip.c_str(),port.c_str());

            int iport;
            std::stringstream portstr(port);
            portstr >> iport;

            bool found=false;
            // check if it is already in list
            std::vector<masterserver::ServerInfo*>::iterator si;
            for ( si = serverlist->begin(); si != serverlist->end(); si++) {
                if ( ((*si)->address == dirip) && ((*si)->port == iport) ) {
                    found=true;
                    break;
                }
            }
            if (found)
                continue;
            ServerInfo * info = new ServerInfo();
            info->address = dirip;
            info->port = iport;
            info->status = ServerInfo::QUERYING;
            serverlist->push_back(info);
            not_queried.push_back(info);
            sendNextQuery(); // XXX first check it is not already in
        } else if ( token == "final") {
            delete msi;
            querying_msdata.erase(s);
            s->destroy();
            break;
        } else {
            delete msi;
            querying_msdata.erase(s);
            s->destroy();
            break; // extra tokens
        }
    }

}

void
ServerQueryThread::sendNextQuery()
{
    if ( querying_server.size() >= 5 ) // max 5 query at time
        return;
    if ( ! not_queried.empty() ) {
        ServerInfo *info = not_queried.back();
        not_queried.pop_back();
        sendQuery(info);
    }
}

void
ServerQueryThread::sendQuery(ServerInfo *server)
{
    if (server->tryNum++ >= 3) { // 3 retrys fixed for now
        server->status = ServerInfo::TIMEOUT;
        sendNextQuery();
        return;
    }

    if (server->ipaddress == network::Address::ANY) {
        server->ipaddress = network::Address::resolve(server->address, server->port);
    }

    std::stringstream serveraddr;
    serveraddr << server->address << ":" << server->port;
    querying_server[serveraddr.str()]=server;
    LOGGER.warning("Querying server [%s]", serveraddr.str().c_str());

    char q[] = "\\status\\final\\";
    server->querystartticks = SDL_GetTicks();
    udpsocket->send(server->ipaddress,q,sizeof(q)-1);

}

void
ServerQueryThread::onDataReceived(network::UDPSocket *s, const network::Address& from, const char *data, const int len)
{
    (void)s;
    std::stringstream fromaddress;
    fromaddress << from.getIP() << ":" << from.getPort();

    std::string str;
    str.append(data,len);

    ServerInfo * server = querying_server[fromaddress.str()];
    if (server) {
        LOGGER.debug("Got data [%s] from server [%s]", str.c_str(), fromaddress.str().c_str());
        parseServerData(server,str);
        querying_server.erase(fromaddress.str());
    } else {
        LOGGER.warning("Received answer from unknown server [%s]", fromaddress.str().c_str());
        querying_server.erase(fromaddress.str()); // Quick & Dirty, should use find()
    }

    sendNextQuery();

}

void
ServerQueryThread::parseServerData(ServerInfo *server, std::string &data)
{
    server->ping = SDL_GetTicks() - server->querystartticks;

    StringTokenizer tokenizer(data, '\\');

    std::string token;
    while( (token = tokenizer.getNextToken()) != "") {
        if(token == "hostname") {
            server->name = tokenizer.getNextToken();
        } else if(token == "mapname") {
            server->map = tokenizer.getNextToken();
        } else if(token == "numplayers") {
            std::stringstream str(tokenizer.getNextToken());
            str >> server->players;
        } else if(token == "maxplayers") {
            std::stringstream str(tokenizer.getNextToken());
            str >> server->maxplayers;
        } else if(token == "protocol") {
            std::stringstream str(tokenizer.getNextToken());
            str >> server->protocol;
        } else if(token == "authentication") {
            server->auth_on = tokenizer.getNextToken().compare("y") == 0 ? true : false;
        } else if(token == "password") {
            server->needs_password = tokenizer.getNextToken().compare("y") == 0 ? true : false;
        } else {
            // handle more tokens...
        }
    }
    server->status = ServerInfo::RUNNING;

}

void
ServerQueryThread::checkTimeOuts()
{
    Uint32 now = SDL_GetTicks();

    if ( querying_msdata.empty() && querying_server.empty() && not_queried.empty()) {
        LOGGER.warning("Stopping queries to servers, no more servers");
        running = false;
        state = STATE_DONE;
        if (udpsocket) {
            udpsocket->destroy();
            udpsocket = nullptr;
        }
        return;
    }

    auto msiter = querying_msdata.begin();
    while (msiter != querying_msdata.end()) {
        if ( now - msiter->second->lastTicks > MS_TIMEOUT ) {
            LOGGER.warning("Masterserver [%s] timeout", msiter->first->getAddress().getIP().c_str());
            delete msiter->second;
            msiter->first->destroy();
            msiter = querying_msdata.erase(msiter);
            sendNextQuery();
        } else {
            msiter++;
        }
    }


    auto i = querying_server.begin();
    while (i != querying_server.end()) {
        if ( i->second->status == ServerInfo::TIMEOUT ) {
            LOGGER.warning("Server [%s] timeout, removing", i->first.c_str());
            i = querying_server.erase(i);
            break; // no more today, needed for invalid iterator
        } else if ( now - i->second->querystartticks > QUERY_TIMEOUT ) {
            LOGGER.warning("Server [%s] timeout, retrying", i->first.c_str());
            sendQuery(i->second);
        }
        i++;
    }
}




//    if(not_queried.empty() && querying.empty()) {
//        state = STATE_DONE;
//        return;
//    }

//    for(std::vector<ServerInfo*>::iterator i = querying.begin();
//            i != querying.end(); ) {
//        if((*i) == server)
//            i = querying.erase(i);
//        else
//            ++i;
//    }


const char*
ServerQueryThread::getStateMessage() const
{
    switch(state) {
        case STATE_NOSERVERS:
            return "No Masterservers configured";
        case STATE_QUERYMASTERSERVER:
            return "Querying Masterserver";
        case STATE_ERROR:
            return "No response from Masterserver";
        case STATE_QUERYSERVERS:
        case STATE_DONE:
            return "No servers found";
    }

    return "Querying Masterserver";
}

} // masterserver

