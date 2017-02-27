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


#include "MiniMapViewAlt.hpp"
#include "Views/Components/MiniMap.hpp"

// MiniMapView
//---------------------------------------------------------------------------
MiniMapViewAlt::MiniMapViewAlt() : GameTemplateView()
{
    assert(this != 0);

    setSearchName("MiniMapViewAlt");
    setTitle("MiniMapViewAlt");
    setSubTitle("");
    setAllowResize(false);
    setDisplayStatusBar(false);
    setVisible(false);
    setAllowMove(true);

    setBordered(false);

    resize(320, 320);
    add(new MiniMap(1,1,318,318));
    //resize(320, 320);
    //add(new MiniMap(1,1,318,318));
} // end MiniMapView::MiniMapView



// init
//---------------------------------------------------------------------------
void MiniMapViewAlt::init()
{
    minMapSize =  64;
    maxMapSize = 480;
} // end MiniMapView::init

void
MiniMapViewAlt::checkResolution(iXY oldResolution, iXY newResolution)
{
    moveTo(iXY(0,newResolution.y-320)); //160
}

// doDraw
//---------------------------------------------------------------------------
void MiniMapViewAlt::doDraw(Surface &viewArea, Surface &clientArea)
{
    // border
    viewArea.drawRect(iRect(0,0,viewArea.getWidth(), viewArea.getHeight()),Color::gray);

    GameTemplateView::doDraw(viewArea, clientArea);
} // end doDraw
