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


#include "ResignView.hpp"
#include "Views/Components/Desktop.hpp"
#include "2D/Palette.hpp"
#include "AreYouSureResignView.hpp"
#include "AreYouSureExitView.hpp"

//---------------------------------------------------------------------------
static void bResign()
{
    Desktop::setVisibility("ResignView", false);
    Desktop::setVisibility("AreYouSureResignView", true);
}

//---------------------------------------------------------------------------
static void bReturnToGame()
{
    Desktop::setVisibility("ResignView", false);
}

//---------------------------------------------------------------------------
static void bExitNetPanzer()
{
    Desktop::setVisibility("ResignView", false);
    Desktop::setVisibility("AreYouSureExitView", true);
}


// ResignView
//---------------------------------------------------------------------------
ResignView::ResignView() : View()
{
    setSearchName("ResignView");
    setTitle("Resign");
    setSubTitle("");

} // end ResignView::ResignView

// init
//---------------------------------------------------------------------------
void ResignView::init()
{
    removeAllButtons();

    setBordered(false);
    setAllowResize(false);
    setDisplayStatusBar(false);

    resize(iXY(480, 80));

    iXY buttonSize(150, 15);
    //int BUTTON_COUNT = 2;
    int x            = (getClientRect().getSize().x - buttonSize.x) / 2;
    int y;
    int yOffset      = buttonSize.y * 2;

    y = 0;
    addButtonCenterText(iXY(x, y), buttonSize.x, "Resign", "Returns to the MainView.", bResign);
    y += yOffset;
    addButtonCenterText(iXY(x, y), buttonSize.x, "Exit netPanzer", "Exits to Windows.", bExitNetPanzer);
    y += yOffset;
    addButtonCenterText(iXY(x, y), buttonSize.x, "Return To Game", "", bReturnToGame);

} // end ResignView::init

// doDraw
//---------------------------------------------------------------------------
void ResignView::doDraw(Surface &viewArea, Surface &clientArea)
{
    iRect r(0, 0, currentscreen->getWidth(), currentscreen->getHeight());

    currentscreen->bltLookup(r, Palette::darkGray256.getColorArray());
    currentscreen->drawButtonBorder(r, Color::lightGreen, Color::darkGreen);

    View::doDraw(viewArea, clientArea);
} // end ResignView::doDraw

// doActivate
//---------------------------------------------------------------------------
void ResignView::doActivate()
{
    init();
    Desktop::setActiveView(this);

} // end ResignView::doActivate
