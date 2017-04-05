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

#include "DisconectedView.hpp"

#include "Interfaces/GameManager.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/MainMenu/MenuTemplateView.hpp"
#include "Views/MainMenu/OptionsTemplateView.hpp"
#include "Classes/ScreenSurface.hpp"

#include "Units/UnitProfileInterface.hpp"
#include "UStyleSelectionView.hpp"

#include "Util/Log.hpp"


void
DisconectedView::buttonOk()
{
    if(gameconfig->quickConnect == true) {
        GameManager::exitNetPanzer();
        return;
    }

    GameManager::drawTextCenteredOnScreen("Loading Main View...", Color::white);
    sprintf(MenuTemplateView::currentMultiView, "GetSessionView");

    // Vlad put all code in here for shutdown.
    //----------------------
    GameManager::quitNetPanzerGame();
    //----------------------

    // Swap to the menu resolution.
    //GameManager::setVideoMode(iXY(640, 480), false);

    GameManager::drawTextCenteredOnScreen("Loading Main View...", Color::white);


    // Must remove the gameView first so that the initButtons detects that
    // and loads the correct buttons.
    Desktop::setVisibilityAllWindows(false);

     // cleaning some vectors and resetting stuff - important on restart!
    UnitProfileInterface::cleaning();
    GameManager::cleaning();
    UnitProfileSprites::clearProfiles();
    UnitProfileInterface::clearProfiles();


    UStyleSelectionView::rstyle_mem = 0;


    Desktop::setVisibility("MainView", true);

    View *v = Desktop::getView("OptionsView");

    if (v != 0) {
        ((OptionsTemplateView *)v)->initButtons();
        ((OptionsTemplateView *)v)->setAlwaysOnBottom(true);
    } else {
        assert(false);
    }
    LOGGER.warning("DisconectedView:: finished disconection");
}


DisconectedView::DisconectedView() : SpecialButtonView()
{
    setSearchName("DisconectedView");
    setTitle("Disconected from server");
    setSubTitle("");
}

void
DisconectedView::init()
{
    removeAllButtons();

    setBordered(true);
    setAllowResize(false);

    resize(screen->getWidth(),screen->getHeight());
    moveTo(0,0);

    int bsize = Surface::getTextLength(" ") * 8;
    addButtonCenterText(iXY((getClientRect().getSizeX()/2)-(bsize/2),
                (getClientRect().getSizeY()/2)+(Surface::getFontHeight() * 2)),
                bsize, "Ok", "", buttonOk);
}

void
DisconectedView::doDraw(Surface &viewArea, Surface &clientArea)
{
    iRect r(min, max);

    viewArea.bltLookup(r, Palette::darkGray256.getColorArray());
    viewArea.bltStringCenter(disconectMsg.c_str(), Color::white);

    View::doDraw(viewArea, clientArea);
}

void
DisconectedView::doActivate()
{
    init();
    Desktop::setActiveView(this);
}
