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


#include "MainMenuView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Interfaces/GameManager.hpp"
#include "Views/GameViewGlobals.hpp"

#include "Views/MainMenu/Multi/HostJoinTemplateView.hpp"
#include "Views/MainMenu/Multi/GetSessionView.hpp"
#include "Interfaces/GameConfig.hpp"

#ifndef PACKAGE_VERSION
	#define PACKAGE_VERSION "testing"
#endif

// MainMenuView
//---------------------------------------------------------------------------
MainMenuView::MainMenuView() : MenuTemplateView()
{
    setSearchName("MainView");
    setTitle("Main");
    setSubTitle("");
    MenuTemplateView::loadMainBackgroundSurface();
    setVisible(true);
} // end MainMenuView::MainMenuView

// doDraw
//---------------------------------------------------------------------------
void MainMenuView::doDraw(Surface &viewArea, Surface &clientArea)
{
    MenuTemplateView::doDrawM(viewArea, clientArea);

    //static char text[] =
    //    "";

    //viewArea.bltStringInBox(bodyTextRect, text, windowTextColor, 12);
} // end MainMenuView::doDraw
/*
void MainMenuView::drawMBack()
{
    MenuTemplateView::loadMainBackgroundSurface();
}
*/


