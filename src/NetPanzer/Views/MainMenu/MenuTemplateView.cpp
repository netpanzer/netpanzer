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

#include <vector>

#include "Views/MainMenu/MenuTemplateView.hpp"
#include "2D/Palette.hpp"
#include "Views/Components/Desktop.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameManager.hpp"
#include "System/Sound.hpp"
#include "Views/Components/ViewGlobals.hpp"
#include "Views/Components/newButton.hpp"
#include "Particles/RadarPingParticle2D.hpp"
#include "Classes/ScreenSurface.hpp"
#include "Particles/Particle2D.hpp"
#include "Particles/ParticleSystem2D.hpp"
#include "2D/PackedSurface.hpp"
#include "Interfaces/GameManager.hpp"
#include "Util/Exception.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Multi/ServerListView.hpp"

#ifndef PACKAGE_VERSION
	#define PACKAGE_VERSION "testing"
#endif

Surface       MenuTemplateView::backgroundSurface;
PackedSurface MenuTemplateView::titlePackedSurface;

//PackedSurface MenuTemplateView::netPanzerLogo;


char MenuTemplateView::currentMultiView[] = "GetSessionView";
char MenuTemplateView::currentView[]      = "";

static void bMain()
{
    Desktop::setVisibilityAllWindows(false);
    Desktop::setVisibility("MainView", true);
}

static void bHost()
{
        gameconfig->hostorjoin = _game_session_host;
        Desktop::setVisibilityAllWindows(false);
        Desktop::setVisibility("HostView", true);
        Desktop::setVisibility("UnitSelectionView", true);
        Desktop::setVisibility("HostOptionsView", true);
        Desktop::setVisibility("MapSelectionView", true);
        Desktop::setVisibility("PlayerNameView", true);
}
static void bJoin()
{
        gameconfig->hostorjoin = _game_session_join;
        Desktop::setVisibilityAllWindows(false);
        Desktop::setVisibility("JoinView", true);
        Desktop::setVisibility("PlayerNameView", true);
        Desktop::setVisibility("IPAddressView", true);
        Desktop::setVisibility("TipsView", true);
        Desktop::setVisibility("ServerListView", true);
        serverlistview->refresh();

}

static void bOptions()
{
    Desktop::setVisibilityAllWindows(false);
    Desktop::setVisibility("OptionsView", true);
}

static void bCredits()
{
    Desktop::setVisibilityAllWindows(false);
    Desktop::setVisibility("CreditsView", true);
}

static void bHelp()
{
    Desktop::setVisibilityAllWindows(false);
    Desktop::setVisibility("HelpView", true);
}

static void bExit()
{
    Desktop::setVisibilityAllWindows(false);
    GameManager::exitNetPanzer();
}

//---------------------------------------------------------------------------
static void bResign()
{
    Desktop::setVisibility("OptionsView", false);
    Desktop::setVisibility("AreYouSureResignView", true);
}

//---------------------------------------------------------------------------
static void bCloseOptions()
{
    GameManager::setNetPanzerGameOptions();

    Desktop::setVisibility("OptionsView", false);
}

//---------------------------------------------------------------------------
static void bExitNetPanzer()
{
    Desktop::setVisibility("OptionsView", false);
    Desktop::setVisibility("AreYouSureExitView", true);
}

// MenuTemplateView
//---------------------------------------------------------------------------
MenuTemplateView::MenuTemplateView() : RMouseHackView()
{
    setSearchName("MenuTemplateView");
    setTitle("MenuTemplate");
    setSubTitle("");

    setAllowResize(false);
    setVisible(false);
    setAllowMove(false);
    setDisplayStatusBar(false);
    setBordered(false);
    setAlwaysOnBottom(true);

    moveTo(iXY(0, 0));
    resize(iXY(800, 600));

    initButtons();

    //curTitleFlashTime  = 0.0f;
    //titleFlashTimeHalf = 0.5;
} // end MenuTemplateView constructor

//---------------------------------------------------------------------------
// initPreGameOptionButtons
void MenuTemplateView::initPreGameOptionButtons()
{
    iXY emptyPos;
    addHorizontal(mainTopButtonsStartPos, 5, std::vector<Component*> {
            new newButton( "MAIN", "Main", emptyPos, 0),
            new newButton( "JOIN", "Join", emptyPos, 0),
            new newButton( "HOST", "Host", emptyPos, 0),
            new newButton( "OPTIONS", "Options", emptyPos, 0),
            new newButton( "CREDITS", "Credits", emptyPos, 0),
            new newButton( "HELP", "Help", emptyPos, 0),
    });
    add(new newButton( "EXITNP", "Exit netPanzer", exitPos, 0));
} // end MenuTemplateView::initPreGameOptionButtons

// initInGameOptionButtons
//---------------------------------------------------------------------------
void MenuTemplateView::initInGameOptionButtons()
{
    if(!gameconfig->quickConnect) {
        add( new newButton( "RESIGN", "Resign", resignPos, 0) );
        add( new newButton( "EXITNETNP", "Exit netPanzer", exitPos, 0) );
    } else {
        add( new newButton( "EXITNETNP", "Exit netPanzer", exitPos, 0) );
    }

    add( new newButton( "CLOSEOPT", "Close Options", returnToGamePos, 0) );
} // end MenuTemplateView::initInGameOptionButtons

// initButtons
//---------------------------------------------------------------------------
void MenuTemplateView::initButtons()
{
    if (Desktop::getVisible("GameView")) {
        initInGameOptionButtons();
    } else {
        initPreGameOptionButtons();
    }
} // end MenuTemplateView::initButtons

// doDraw
//---------------------------------------------------------------------------
void MenuTemplateView::doDraw(Surface &viewArea, Surface &clientArea)
{
    //setWorldRect();
    if (Desktop::getVisible("GameView")) {
	// When ingame, tint the game into gray

        //clientArea.BltRoundRect(getClientRect(), 10, Palette::darkGray256.getColorArray());
        //clientArea.FillRoundRect(getClientRect(), 10, Color::white); // esc in game
        clientArea.RoundRect(MenuRect, 10, Color::gray);
        clientArea.FillRoundRect(MenuRect, 10, Color::white);
        //clientArea.drawWindowsBorder();

    } else {
        screen->fill(0);
        loadBackgroundSurface();
		// Set the following to get does exist.
        if (backgroundSurface.getNumFrames() > 0) {
            backgroundSurface.blt(viewArea, 0, 0);
        } else {
            throw Exception("Where is the background surface?");
        }
        clientArea.BltRoundRect(MenuRect, 10, Palette::gray256.getColorArray());
        clientArea.FillRoundRect(MenuRect, 10, Color::white);
        clientArea.RoundRect(MenuRect, 10, Color::gray);

        static char text[] =
        "NetPanzer " PACKAGE_VERSION;
        clientArea.bltString(10, 580, text, Color::gray);
        // I don't like them so I delete them!
        //titlePackedSurface.blt(clientArea, bodyTextRect.min.x, 390);
        //titlePackedSurface.bltBlend(clientArea, bodyTextRect.min.x, bodyTextRect.max.y-50, Palette::colorTable6040);
    }
/*
    static char text[] =
        "netPanzer " PACKAGE_VERSION;
        clientArea.bltString(10, 590, text, Color::gray);
*/
    //LOGGER.info("View is %s", currentView);
    View::doDraw(viewArea, clientArea);
} // end doDraw


void MenuTemplateView::doDrawM(Surface &viewArea, Surface &clientArea)  // start page
{
    //setWorldRect();
    if (Desktop::getVisible("GameView")) {
	// When ingame, tint the game into gray

        clientArea.BltRoundRect(getClientRect(), 10, Palette::red256.getColorArray());
        clientArea.FillRoundRect(getClientRect(), 10, Color::red); // esc in game
        //clientArea.RoundRect(MenuRect, 10, Color::gray);
        clientArea.drawWindowsBorder();

    } else {
        screen->fill(0);
        loadMainBackgroundSurface();
		// Set the following to get does exist.
        if (backgroundSurface.getNumFrames() > 0) {
            backgroundSurface.blt(viewArea, 0, 0);
        } else {
            throw Exception("Where is the background surface?");
        }
        clientArea.BltRoundRect(MenuRect, 10, Palette::red256.getColorArray());
        clientArea.FillRoundRect(MenuRectStart, 3, Color::red);
        clientArea.RoundRect(MenuRectStart, 3, Color::gray);

        // I don't like them so I delete them!
        //titlePackedSurface.blt(clientArea, bodyTextRect.min.x, 390);
        //titlePackedSurface.bltBlend(clientArea, bodyTextRect.min.x, bodyTextRect.max.y-50, Palette::colorTable6040);
    }

    static char text[] =
        "NetPanzer " PACKAGE_VERSION;
        clientArea.bltString(10, 580, text, Color::gray);

    View::doDraw(viewArea, clientArea);
} // end doDrawM


void MenuTemplateView::doDrawAlt(Surface &viewArea, Surface &clientArea) //help view
{
    //setWorldRect();
    if (Desktop::getVisible("GameView")) {
	// When ingame, tint the game into gray

        //clientArea.BltRoundRect(getClientRect(), 10, Palette::gray256.getColorArray());
        //clientArea.FillRoundRect(getClientRect(), 10, Color::white); // esc in game
             //clientArea.RoundRect(MenuRect, 10, Color::gray);
        //clientArea.drawWindowsBorder();

    } else {
        screen->fill(0);
        loadBackgroundSurface();
		// Set the following to get does exist.
        if (backgroundSurface.getNumFrames() > 0) {
            backgroundSurface.blt(viewArea, 0, 0); // del for a very compact view
        } else {
            throw Exception("Where is the background surface?");
        }
        clientArea.BltRoundRect(MenuRect, 10, Palette::gray256.getColorArray());
        clientArea.FillRoundRect(MenuRect, 10, Color::white); // gray
        clientArea.RoundRect(MenuRect, 10, Color::gray);

        }

    static char text[] =
        "NetPanzer " PACKAGE_VERSION;
        clientArea.bltString(10, 590, text, Color::gray);

    View::doDraw(viewArea, clientArea);
} // end doDraw






// doActivate
//---------------------------------------------------------------------------
void MenuTemplateView::doActivate()
{
    // Make the activating view active, redo this please!
    sprintf(currentView, "%s", searchName);
    Desktop::setActiveView(searchName);

    //loadBackgroundSurface();
    //loadTitleSurface();
    //loadNetPanzerLogo();
} // end doActivate

// loadBackgroundSurface
//---------------------------------------------------------------------------
void MenuTemplateView::loadBackgroundSurface()
{
    doLoadBackgroundSurface("pics/backgrounds/menus/menu/defaultMB.bmp");
} // end MenuTemplateView::loadBackgroundSurface
void MenuTemplateView::loadMainBackgroundSurface()
{
    doLoadBackgroundSurface("pics/backgrounds/menus/menu/defaultMBstart.bmp");
} // end MenuTemplateView::loadBackgroundSurface
// doLoadBackgroundSurface
//---------------------------------------------------------------------------
void MenuTemplateView::doLoadBackgroundSurface(const std::string& string)
{
    backgroundSurface.loadBMP(string.c_str());
} // end MenuTemplateView::doLoadBackgroundSurface

// loadTitleSurface
//---------------------------------------------------------------------------
void MenuTemplateView::loadTitleSurface()
{
    //doLoadTitleSurface("mainTitle");
} // end MenuTemplateView::loadTitleSurface

// doLoadTitleSurface
//---------------------------------------------------------------------------
void MenuTemplateView::doLoadTitleSurface(const std::string& string)
{
    /*
    curTitleFlashTime  = 0.0f;
    titleFlashTimeHalf = 2.5;

    std::string pakString = "pics/backgrounds/menus/menu/pak/";
    pakString += string;
    pakString += ".pak";

    titlePackedSurface.load(pakString);
    */
} // end MenuTemplateView::doLoadTitleSurface

// doDeactivate
//---------------------------------------------------------------------------
void MenuTemplateView::doDeactivate()
{
} // end doDeactivate

//---------------------------------------------------------------------------
void MenuTemplateView::loadNetPanzerLogo()
{} // end MenuTemplateView::loadNetPanzerLogo

// processEvents
//---------------------------------------------------------------------------
void MenuTemplateView::processEvents()
{
} // end MenuTemplateView::processEvents

void MenuTemplateView::onComponentClicked(Component* c)
{
    std::string cname = c->getName();
    if ( !cname.compare("Button.MAIN") )
    {
        bMain();
    }
    else if ( !cname.compare("Button.JOIN") )
    {
        bJoin();
    }
    else if ( !cname.compare("Button.HOST") )
    {
        bHost();
    }
    else if ( !cname.compare("Button.OPTIONS") )
    {
        bOptions();
    }
    else if ( !cname.compare("Button.CREDITS") )
    {
        bCredits();
    }
    else if ( !cname.compare("Button.HELP") )
    {
        bHelp();
    }
    else if ( !cname.compare("Button.EXITNP") )
    {
        bExit();
    }
    else if ( !cname.compare("Button.EXITNETNP") )
    {
        bExitNetPanzer();
    }
    else if ( !cname.compare("Button.RESIGN") )
    {
        bResign();
    }
    else if ( !cname.compare("Button.CLOSEOPT") )
    {
        bCloseOptions();
    }
}
