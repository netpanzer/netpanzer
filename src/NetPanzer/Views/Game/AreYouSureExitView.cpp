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


#include "AreYouSureExitView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Components/Button.hpp"
#include "2D/Palette.hpp"
#include "Classes/ScreenSurface.hpp"
#include "Interfaces/GameManager.hpp"

#include "Views/GameViewGlobals.hpp"

//---------------------------------------------------------------------------
static void bYES()
{
    GameManager::drawTextCenteredOnScreen("Exiting", Color::white);

    GameManager::exitNetPanzer();
}

//---------------------------------------------------------------------------
static void bNO()
{
    Desktop::setVisibility("AreYouSureExitView", false);
    //Desktop::setVisibility("ResignView", true);
}

// AreYouSureExitView
//---------------------------------------------------------------------------
AreYouSureExitView::AreYouSureExitView() : SpecialButtonView()
{
    setSearchName("AreYouSureExitView");
    setTitle("Exit netPanzer");
    setSubTitle("");
    loaded = false;
	} // end AreYouSureExitView::AreYouSureExitView

// init
//---------------------------------------------------------------------------
void AreYouSureExitView::init()
{
    removeAllButtons();

    setBordered(false);
    setAllowResize(false);
    setDisplayStatusBar(false);

    //moveTo(iXY(0, 0));
    //resize(iXY(800, 600));
    moveTo(bodyTextRect.min);
    resize(bodyTextRect.getSize());

    int x = (getClientRect().getSize().x - (141 * 2 + 20)) / 2;
    int y = getClientRect().getSize().y/2 + 10;
    add( Button::createSpecialButton( "YES", "YES", iXY(x, y)) );
    x += 136 + 20;
    add( Button::createSpecialButton( "NO", "NO", iXY(x, y)) );
    loaded = true;
} // end AreYouSureExitView::init

// doDraw
//---------------------------------------------------------------------------
void AreYouSureExitView::doDraw(Surface &viewArea, Surface &clientArea)
{
    viewArea.bltLookup(getClientRect(), Palette::darkGray256.getColorArray());
    //viewArea.drawButtonBorder(r, Color::lightGreen, Color::darkGreen);

    viewArea.bltStringCenterMin30("Are you sure you wish to exit netPanzer?", Color::white);

    View::doDraw(viewArea, clientArea);
} // end AreYouSureExitView::doDraw

// doActivate
//---------------------------------------------------------------------------
void AreYouSureExitView::doActivate()
{
    if ( ! loaded )
    {
	init();
    }
    Desktop::setActiveView(this);

} // end AreYouSureExitView::doActivate

void AreYouSureExitView::onComponentClicked(Component* c)
{
    std::string cname = c->getName();
    if ( !cname.compare("Button.YES") )
    {
        bYES();
    }
    else if ( !cname.compare("Button.NO") )
    {
        bNO();
    }
}
