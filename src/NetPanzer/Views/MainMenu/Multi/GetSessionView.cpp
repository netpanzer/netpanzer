/*
Copyright (C) 1998 Pyrosoft Inc. (www.pyrosoftgames.com), Matthew Bogue

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


#include "GetSessionView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Components/Button.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/MouseInterface.hpp"
#include "Views/GameViewGlobals.hpp"

#include "IPAddressView.hpp"
#include "ServerListView.hpp"

#include "Classes/Network/NetworkServer.hpp"

static int previousSessionType = _game_session_join;


/////////////////////////////////////////////////////////////////////////////
// Button functions.
/////////////////////////////////////////////////////////////////////////////

static void bBack()
{
    Desktop::setVisibilityAllWindows(false);
    Desktop::setVisibility("MainView", true);
}

void bNext()
{
    if (gameconfig->hostorjoin == _game_session_host) {
        Desktop::setVisibilityAllWindows(false);

        Desktop::setVisibility("HostView", true);
        Desktop::setVisibility("UnitSelectionView", true);
        Desktop::setVisibility("HostOptionsView", true);
        Desktop::setVisibility("MapSelectionView", true);
        Desktop::setVisibility("PlayerNameView", true);

        SERVER->openSession();
    } else if (gameconfig->hostorjoin == _game_session_join) {
        Desktop::setVisibility("JoinView", true);
        Desktop::setVisibility("PlayerNameView", true);
        Desktop::setVisibility("TipsView", true);
        Desktop::setVisibility("IPAddressView", true);
        Desktop::setVisibility("ServerListView", true);
        serverlistview->refresh();
    }
}

static void bHost()
{
    previousSessionType = gameconfig->hostorjoin;

    gameconfig->hostorjoin = _game_session_host;
}

static void bJoin()
{
    previousSessionType = gameconfig->hostorjoin;

    gameconfig->hostorjoin = _game_session_join;
}


// GetSessionView
//---------------------------------------------------------------------------
GetSessionView::GetSessionView() : MenuTemplateView()
{
    setSearchName("GetSessionView");
    setTitle("Select Session");
    setSubTitle("");

    add( Button::createSpecialButton( "JOIN", "Join", joinPos) );
    add( Button::createSpecialButton( "HOST", "Host", hostPos) );
} // end GetSessionView::GetSessionView

// doDraw
//---------------------------------------------------------------------------
void GetSessionView::doDraw(Surface &viewArea, Surface &clientArea)
{
    if (previousSessionType != gameconfig->hostorjoin) {
        loadTitleSurface();
    }

    MenuTemplateView::doDraw(viewArea, clientArea);

    drawInfo(clientArea);

} // end GetSessionView::doDraw

//---------------------------------------------------------------------------
void GetSessionView::drawInfo(Surface &dest)
{
    int connectionType = gameconfig->hostorjoin;

    if (connectionType == _game_session_host) {
        drawHostInfo(dest, bodyTextRect);
    } else if (connectionType == _game_session_join) {
        drawJoinInfo(dest, bodyTextRect);
    }
}

//---------------------------------------------------------------------------
void GetSessionView::drawHostInfo(Surface &dest, const iRect &rect)
{
    static char	tcpipInfo[] =
        "HOSTING INTERNET GAMES\n"
        "\n"
        "Over the Internet varying numbers of players can be "
        "supported depending on connection speed and quality. "
        "\n"
        "Maximum Internet Player Recommendations:\n"
        "\n"
        "  28.8 modem:      4\n"
        "  33.6 modem:      6\n"
        "  56.6 modem:      8\n"
        "  ISDN or faster: 25\n"
        "\n"
        "Click the Next button to proceed";

    dest.bltStringInBox(rect, tcpipInfo, Color::white, 12);
}

//---------------------------------------------------------------------------
void GetSessionView::drawJoinInfo(Surface &dest, const iRect &rect)
{
    static char	tcpipInfo[] =
        "JOINING LAN OR INTERNET GAMES\n"
        "\n"
        "There are three ways to join a game:\n"
        "- you can pick a server from the in-game Lobby.\n"
        "- you can manually specify the IP address of the server, which is "
        "useful for LAN games.\n"
        "- use external browsers such as qstat to get a list of "
        "available Internet servers, and then run 'netpanzer -c <server>' to "
        "connect directly.\n"
        "\n"
        "Click the Next button to proceed.";


    dest.bltStringInBox(rect, tcpipInfo, Color::white, 12);
}

// doActivate
//---------------------------------------------------------------------------
void GetSessionView::doActivate()
{
    MenuTemplateView::doActivate();

    sprintf(currentMultiView, "%s", searchName);

} // end GetSessionView::doActivate

// loadBackgroundSurface
//---------------------------------------------------------------------------
void GetSessionView::loadBackgroundSurface()
{
} // end GetSessionView::loadBackgroundSurface

// loadTitleSurface
//---------------------------------------------------------------------------
void GetSessionView::loadTitleSurface()
{
    if (gameconfig->hostorjoin == _game_session_host) {
        doLoadTitleSurface("hostTitle");
    } else if (gameconfig->hostorjoin == _game_session_join) {
        doLoadTitleSurface("joinTitle");
    }

} // end GetSessionView::loadTitleSurface

void GetSessionView::onComponentClicked(Component* c)
{
    std::string cname = c->getName();
    if ( !cname.compare("Button.JOIN") )
    {
        bJoin();
    }
    else if ( !cname.compare("Button.HOST") )
    {
        bHost();
    }
    else if ( !cname.compare("Button.NEXT") )
    {
        bNext();
    }
    else if ( !cname.compare("Button.BACK") )
    {
        bBack();
    }
}
