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

#include <string>
#include <sstream>
#include <stdexcept>
#include <SDL2/SDL.h>
#include "Heartbeat.hpp"
#include "Core/NetworkGlobals.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Util/StringTokenizer.hpp"
#include "Util/StringUtil.hpp"
#include "Util/Log.hpp"

using namespace network;

#define MSQUERY_TIMEOUT 5 * 1000
#define HEARTBEAT_INTERVAL 5 * 60 * 1000

class MasterserverInfo {
public:
    MasterserverInfo() : timer(MSQUERY_TIMEOUT){ };
    ~MasterserverInfo(){};
    inline void touch() { timer.reset(); };
    std::string recdata;
    NTimer timer;
    
};

Heartbeat::Heartbeat() : nextHeartbeat(HEARTBEAT_INTERVAL)
{
    StringTokenizer mstokenizer(*GameConfig::server_masterservers, ',');
    std::string servname;
    while( !(servname = removeSurroundingSpaces(mstokenizer.getNextToken())).empty()) {
        try {
            Address addr = Address::resolve(servname, MASTERSERVER_PORT, true, false);
            mslist.push_back(addr);
        } catch (NetworkException& e) {
            LOGGER.warning("Bad masterserver address: %s", e.what());
        }
    }
    
    std::stringstream msg;
    msg << "\\heartbeat\\gamename\\netpanzer\\port\\" << GameConfig::server_port
        << "\\protocol\\" << NETPANZER_PROTOCOL_VERSION
        << "\\final\\";
    
    hb_message=msg.str();
    startHeartbeat();
    
}

Heartbeat::~Heartbeat()
{
    if ( !masterservers.empty() ) {
        std::map<TCPSocket *, MasterserverInfo *>::iterator msiter;
        for (msiter=masterservers.begin(); msiter!=masterservers.end(); msiter++) {
            delete msiter->second;
            msiter->first->destroy();
        }
        masterservers.clear();
    }
    mslist.clear();
}

void
Heartbeat::checkHeartbeat()
{
    Uint32 now = SDL_GetTicks();

    if ( !masterservers.empty() ) {
        std::map<TCPSocket *, MasterserverInfo *>::iterator msiter;
        for (msiter=masterservers.begin(); msiter!=masterservers.end(); msiter++) {
            if ( msiter->second->timer.isTimeOut(now) ) {
                LOGGER.warning("Masterserver timeout [%s]", msiter->first->getAddress().getIP().c_str());
                delete msiter->second;
                msiter->first->destroy();
                masterservers.erase(msiter);
                break; // no more checking today, next time, iterator invalid
            }
        }
    } else if ( nextHeartbeat.isTimeOut(now) ) {
        startHeartbeat();
    }

}

void
Heartbeat::startHeartbeat()
{
    std::vector<Address>::iterator iter = mslist.begin();
    Uint32 now = SDL_GetTicks();
    while ( iter != mslist.end() ) {
        TCPSocket *s = nullptr;
        MasterserverInfo *msi = nullptr;
        try {
            s = new TCPSocket(*iter, this);
            msi = new MasterserverInfo();
            msi->timer.reset(now);
            masterservers[s]=msi;
        } catch (NetworkException& e) {
            LOGGER.warning("Error '%s' connecting to masterserver '%s'", e.what(), (*iter).getIP().c_str());
            if (msi)
                delete msi;
        }
        iter++;
    }
    nextHeartbeat.reset();
}

void
Heartbeat::onConnected(TCPSocket *so)
{
    LOGGER.debug("Masterserver connected [%s]", so->getAddress().getIP().c_str());
    masterservers[so]->touch();
    so->send(hb_message.c_str(), hb_message.size());
}

void
Heartbeat::onDisconnected(network::TCPSocket *s)
{
    LOGGER.debug("Masterserver disconected [%s]", s->getAddress().getIP().c_str());
    delete masterservers[s];
    masterservers.erase(s);
}

void
Heartbeat::onSocketError(TCPSocket *so)
{
    LOGGER.debug("Masterserver socket error [%s]", so->getAddress().getIP().c_str());
    delete masterservers[so];
    masterservers.erase(so);
}

void
Heartbeat::onDataReceived(TCPSocket *so, const char *data, const int len)
{
    std::string str;
    
    MSMapIterator i = masterservers.find(so);
    if ( i == masterservers.end() ) {
        LOGGER.warning("Received data from Unknown masterserver [%s]", so->getAddress().getIP().c_str());
        so->destroy();
        return;
    }
    
    MasterserverInfo * msi = i->second;
    msi->touch();
    str = msi->recdata;
    str.append(data,len);
    
    if (str[0] != '\\') {
        LOGGER.warning("Bad answer from masterserver, missing initial separator [%s]", so->getAddress().getIP().c_str());
        delete msi;
        masterservers.erase(so);
        so->destroy();
        return; // invalid answer, needs '\\' at beggining;
    }
    
    std::string::size_type strpos = str.find('\\',1);
    if ( strpos == std::string::npos ) {
        msi->recdata=str;
        return; // unfinished token, needs '\\' at end
    }
    
    std::string token = str.substr(1,strpos-1);
    if ( !token.empty() ) {
        if ( token == "error" ) {
            std::string::size_type msgend = str.find('\\',strpos+1);
            if ( msgend == std::string::npos ) {
                return; // need '\\' at end, continue with the socket
            } else if ( msgend == strpos+1 ) {
                LOGGER.warning("Masterserver returns empty error");
            } else {
                LOGGER.warning("Masterserver returns error: '%s'", str.substr(strpos+1,msgend-strpos-1).c_str());
            }
            return;
        } else if ( token == "final") {
            LOGGER.debug("Masterserver answer ok, disconnecting [%s]", so->getAddress().getIP().c_str());
        } else {
            LOGGER.warning("Masterserver sent unknown answer: '%s'", token.c_str());
            msi->recdata=str.substr(strpos); // includes the initial '\\'
            return; // continues with the socket;
        }
        
        delete msi;
        masterservers.erase(so);
        so->destroy();
    }

}





