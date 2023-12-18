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


#include <ctype.h>
#include <sstream>
#include "HostJoinTemplateView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Components/newButton.hpp"
#include "HostJoinTemplateView.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/PlayerGameManager.hpp"
#include "HostView.hpp"
#include "Util/Exception.hpp"
#include "MapSelectionView.hpp"
#include "PlayerNameView.hpp"
#include "2D/Palette.hpp"
#include "Views/GameViewGlobals.hpp"
#include "IPAddressView.hpp"
#include "ServerListView.hpp"
#include "TipsView.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Resources/ResourceManager.hpp"

char HostJoinTemplateView::gameTypeBuf[256];

Surface playerColor;

int cDarkBlue;
int cLightBlue;
int cLightGreen;
int cOrange;

/////////////////////////////////////////////////////////////////////////////
// Button functions.
/////////////////////////////////////////////////////////////////////////////

static void bPlay()
{
    if ( GameConfig::player_name->length() == 0 )
        return;

    // Check a few things which should be ok.
    if (strlen(HostJoinTemplateView::gameTypeBuf) == 0) {
        return;
    }
    if (MapSelectionView::curMap == -1) {
        return;
    }
    if (gameconfig->hostorjoin == _game_session_join &&
        strcmp(IPAddressView::szServer.getString(), "") == 0)
        return;

    // Close all menu views.
    Desktop::setVisibilityAllWindows(false);

    if(gameconfig->hostorjoin == _game_session_join) {
        gameconfig->serverConnect = IPAddressView::szServer.getString();
        IPAddressView::szServer.setString("");
    }

    serverlistview->endQuery();

    MenuTemplateView::backgroundSurface.free();

    ResourceManager::updateFlagData( 0,
                                     GameConfig::player_flag_data,
                                     sizeof(GameConfig::player_flag_data) );

    PlayerGameManager* manager = (PlayerGameManager*) gamemanager;
    manager->launchMultiPlayerGame();
}

// HostJoinTemplateView
//---------------------------------------------------------------------------
HostJoinTemplateView::HostJoinTemplateView() : MenuTemplateView()
{
    setSearchName("HostJoinTemplateView");
    setTitle("");
    setSubTitle("");
    add( new newButton( "PLAY", "Play", playPos, 1) );
} // end HostJoinTemplateView constructor

// doDraw
//---------------------------------------------------------------------------
void HostJoinTemplateView::doDraw(Surface &viewArea, Surface &clientArea)
{
    MenuTemplateView::doDraw(viewArea, clientArea);

    //View::doDraw(viewArea, clientArea);
} // end doDraw

// doActivate
//---------------------------------------------------------------------------
void HostJoinTemplateView::doActivate()
{
    MenuTemplateView::doActivate();

    sprintf(currentMultiView, "%s", searchName);
} // end doActivate

// loadBackgroundSurface
//---------------------------------------------------------------------------
void HostJoinTemplateView::loadBackgroundSurface()
{
    MenuTemplateView::loadBackgroundSurface();
} // end HostJoinTemplateView::loadBackgroundSurface

void HostJoinTemplateView::onComponentClicked(Component* c)
{
    std::string cname = c->getName();
    if ( !cname.compare("Button.PLAY") )
    {
        bPlay();
    }
    else
    {
        MenuTemplateView::onComponentClicked(c);
    }

}
