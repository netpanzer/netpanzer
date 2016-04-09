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


#include "MiniMapView.hpp"
#include "Views/Components/MiniMap.hpp"

// MiniMapView
//---------------------------------------------------------------------------
MiniMapView::MiniMapView() : GameTemplateView()
{
    assert(this != 0);

    setSearchName("MiniMapView");
    setTitle("MiniMapView");
    setSubTitle("");
    setAllowResize(false);
    setDisplayStatusBar(false);
    setVisible(false);
    setAllowMove(true);

    setBordered(false);
    
    resize(160, 160);
    add(new MiniMap(1,1,158,158));
} // end MiniMapView::MiniMapView

// init
//---------------------------------------------------------------------------
void MiniMapView::init()
{
    minMapSize =  64;
    maxMapSize = 480;
} // end MiniMapView::init

void
MiniMapView::checkResolution(iXY oldResolution, iXY newResolution)
{
    moveTo(iXY(0,newResolution.y-160));
}

// doDraw
//---------------------------------------------------------------------------
void MiniMapView::doDraw(Surface &viewArea, Surface &clientArea)
{
    // border
    viewArea.drawRect(iRect(0,0,viewArea.getWidth(), viewArea.getHeight()),Color::gray);
    
    GameTemplateView::doDraw(viewArea, clientArea);
} // end doDraw
