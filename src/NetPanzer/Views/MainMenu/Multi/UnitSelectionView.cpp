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

#include "UnitSelectionView.hpp"
#include "Views/Game/GameView.hpp"
#include "Views/GameViewGlobals.hpp"

// UnitSelectionView
//---------------------------------------------------------------------------
UnitSelectionView::UnitSelectionView() : View()
{
    setSearchName("UnitSelectionView");
    setTitle("Selects Your Units");
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setDisplayStatusBar(true);
    setVisible(false);

    moveTo(bodyTextRect.min);

    resize(bodyTextRect.getSize());

    //setScrollBar(true);

    maxYOffset =  0;

    // Define the scrollBar fot this view.
    scrollBar = new ScrollBar(ScrollBar::HORIZONTAL, 0, 1, 0, 100);

    add(scrollBar);

} // end UnitSelectionView::UnitSelectionView

// doDraw
//---------------------------------------------------------------------------
void UnitSelectionView::doDraw(Surface &viewArea, Surface &clientArea)
{
    char strBuf[256];
    sprintf(strBuf, "%d", scrollBar->getValue());
    clientArea.bltStringCenter(strBuf, Color::white);

    View::doDraw(viewArea, clientArea);

} // end UnitSelectionView::doDraw
