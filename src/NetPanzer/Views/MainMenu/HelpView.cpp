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

#include "HelpView.hpp"
#include "Views/Components/Desktop.hpp"

#include "Views/Game/GameView.hpp"
#include "Views/GameViewGlobals.hpp"

//#include "Interfaces/GameManager.hpp"
//#include "Interfaces/GameConfig.hpp"


// HelpView
//---------------------------------------------------------------------------
HelpView::HelpView() : MenuTemplateView()
{
    setSearchName("HelpView");
    setTitle("Help Information");
    setSubTitle("");
    setAllowResize(false);
    setAllowMove(false);
    //setVisible(true);
} // end HelpView::HelpView

// doDraw
//---------------------------------------------------------------------------
void HelpView::doDraw(Surface &viewArea, Surface &clientArea)
{
    MenuTemplateView::doDrawAlt(viewArea, clientArea);
}
 // end HelpView::doDraw

// doActivate
//---------------------------------------------------------------------------
void HelpView::doActivate()
{
    MenuTemplateView::doActivate();
    Desktop::setVisibilityNoDoAnything("HelpScrollView", true);
} // end HelpView::doActivate

// doDeactivate
//---------------------------------------------------------------------------
void HelpView::doDeactivate()
{
} // end HelpView::doDeactivate

// loadTitleSurface
//---------------------------------------------------------------------------
void HelpView::loadTitleSurface()
{
    //doLoadTitleSurface("helpTitle");
} // end HelpView::loadTitleSurface
