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
#ifndef __SERVERLISTVIEW_HPP__
#define __SERVERLISTVIEW_HPP__

#include "Views/Components/View.hpp"
#include "MasterServer/ServerInfo.hpp"
#include "MasterServer/ServerList.hpp"
#include "MasterServer/ServerQueryThread.hpp"

class ServerListView : public View
{
public:
    ServerListView();
    virtual ~ServerListView();

    void refresh();
    void endQuery();
    masterserver::ServerList getViewModel();

    virtual void doDraw(Surface& windowArea, Surface& clientArea);
    virtual int lMouseUp(const iXY& up_pos, const iXY& down_pos);

private:
    masterserver::ServerList serverlist;
    masterserver::ServerQueryThread* queryThread;

    Surface lock_image;
    Surface auth_image;

    static void buttonRefresh();
};

extern ServerListView* serverlistview; // XXX ugly

#endif

