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


#include "ServerListView.hpp"

#include <iostream>
#include <sstream>
#include "IPAddressView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "MasterServer/ServerInfo.hpp"
#include "Core/NetworkGlobals.hpp"
// XXX ultrahack
#include "Classes/ScreenSurface.hpp"
#include "Views/Components/Desktop.hpp"

#include "Views/Game/LoadingView.hpp"

ServerListView* serverlistview = 0;

ServerListView::ServerListView()
    : queryThread(0)
{
    setSearchName("ServerListView");
    setTitle("Servers");
    setSubTitle(" Name                         Players/max     Map          Ping   ");

    moveTo(bodyTextRect.min + iXY(0, 205));
    resizeClientArea(bodyTextRect.max - bodyTextRect.min - iXY(5,220));

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);

    addButtonCenterText(iXY(getClientRect().getSizeX()-80,
                getClientRect().getSizeY() - Surface::getFontHeight() * 2),
            80, "Refresh", "", buttonRefresh);

    // XXX ugly
    serverlistview = this;

    lock_image.loadBMP("pics/default/lock.bmp");
    auth_image.loadBMP("pics/default/auth.bmp");
}

ServerListView::~ServerListView()
{
    delete queryThread;
    serverlistview = 0;
}

void
ServerListView::refresh()
{
    if ( queryThread ) {
        if (queryThread->isRunning())
            return;
        else
            delete queryThread;
    }
    queryThread = 0;

    // don't clear before the delete or after the new, as the thread contains
    // pointers to the serverlist
    for(std::vector<masterserver::ServerInfo*>::iterator i = serverlist.begin();
            i != serverlist.end(); ++i)
        delete *i;
    serverlist.clear();

    bool mylock = false;
    if ( ! screen->getDoesExist() ) {
        screen->lock();
        mylock = true;
    }
    Desktop::draw(*screen); // XXX ultrahack
    if (mylock) {
        screen->unlock();
    }
    screen->render();
    queryThread = new masterserver::ServerQueryThread(&serverlist);
}

void
ServerListView::endQuery()
{
    if (queryThread) {
        delete queryThread;
        queryThread=0;
    }
}

void
ServerListView::buttonRefresh()
{
    serverlistview->refresh();
}

masterserver::ServerList
ServerListView::getViewModel()
{
    masterserver::ServerList filteredList;
    for (masterserver::ServerInfo* serverInfoPtr : serverlist) {
        if (serverInfoPtr->protocol < NETPANZER_PROTOCOL_VERSION) {
            // skipped
            // We want to incentivize players to upgrade.
            // But we don't want to punish players for running a newer version by cluttering the server list.
            // So if server is too old - just ignore it.
        } else {
            filteredList.push_back(serverInfoPtr);
        }
    }
    return filteredList;
}

void
ServerListView::doDraw(Surface& windowArea, Surface& clientArea)
{
    clientArea.fill(Color::black);

    if(queryThread && queryThread->isRunning()) {
        queryThread->checkTimeOuts();
    }

    masterserver::ServerList viewModel = getViewModel();

    if(viewModel.empty()) {
        const char* msg;
        if ( queryThread ) {
            msg = queryThread->getStateMessage();
        } else {
            msg = "Resolving masterserver address";
        }
        clientArea.bltString(0, 0, msg, Color::white);
        View::doDraw(windowArea, clientArea);
        return;
    }

    unsigned int y = 0;
    for(masterserver::ServerInfo* serverPtr : viewModel) {
        const masterserver::ServerInfo& server = *serverPtr;

        if(server.status == masterserver::ServerInfo::QUERYING) {
            clientArea.bltString(0,   y, server.address.c_str(), Color::gray);
            clientArea.bltString(140, y, "querying...", Color::gray);
        } else if(server.status == masterserver::ServerInfo::TIMEOUT) {
            clientArea.bltString(0,   y, server.address.c_str(), Color::gray);
            clientArea.bltString(140, y, "timeout", Color::gray);
        } else if(server.protocol != NETPANZER_PROTOCOL_VERSION) {
            clientArea.bltString(0,   y, server.address.c_str(), Color::gray);
            clientArea.bltString(140, y, getNetpanzerProtocolMessage(server.protocol), Color::gray);
        } else {
            std::stringstream playerstr;
            playerstr << server.players << "/" << server.maxplayers;

            std::stringstream pingstr;
            pingstr << server.ping;

            std::stringstream servaddr;
            servaddr << server.address << ':' << server.port;

            Uint8 textcolor = Color::white;

            if (servaddr.str()==IPAddressView::szServer.getString()) {
                textcolor = Color::yellow;
                clientArea.fillRect(
                    iRect(0,y,clientArea.getWidth(),y+Surface::getFontHeight()),
                    Color::blue);
            }

            char ssn[44];
            SDL_strlcpy(ssn, server.name.c_str(), sizeof(ssn));

            if ( server.auth_on )
            {
                auth_image.blt(clientArea, 0, y);
            }

            if ( server.needs_password && (server.auth_on == false) )
            {
                lock_image.blt(clientArea, 0, y);
            }

            clientArea.bltString(8,   y, ssn, textcolor);
            clientArea.bltString(350, y, playerstr.str().c_str(), textcolor);
            clientArea.bltString(400, y, server.map.c_str(), textcolor);
            clientArea.bltString(550, y, pingstr.str().c_str(), textcolor);

        }

        y += Surface::getFontHeight();
        if(y >= clientArea.getHeight())
            break;
    }

    View::doDraw(windowArea, clientArea);
}

int
ServerListView::lMouseUp(const iXY& down_pos, const iXY& up_pos)
{
    if(down_pos.x < 0 || down_pos.y < 0 || up_pos.x < 0 || up_pos.y < 0) {
        return View::lMouseUp(down_pos, up_pos);
    }

    int listpos = down_pos.y / Surface::getFontHeight();
    masterserver::ServerList viewModel = getViewModel();
    if(listpos >= int(viewModel.size()) ||
            viewModel[listpos]->status != masterserver::ServerInfo::RUNNING) {
        return View::lMouseUp(down_pos, up_pos);
    }

    const masterserver::ServerInfo& server = *(viewModel[listpos]);
    std::stringstream addr;
    addr << server.address << ':' << server.port;
    IPAddressView::szServer.setString(addr.str());

    LoadingView * lv = static_cast<LoadingView*>(Desktop::getView("LoadingView"));
    if ( lv )
    {
        lv->setNeedPassword(server.needs_password);
    }

    return View::lMouseUp(down_pos, up_pos);
}

