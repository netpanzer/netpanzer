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


#include "IPAddressView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/GameViewGlobals.hpp"

cInputFieldString IPAddressView::szServer;

// IPAddressView
//---------------------------------------------------------------------------
IPAddressView::IPAddressView() : View()
{
    setSearchName("IPAddressView");
    setTitle("Server IP Address");
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);

    moveTo(iXY(bodyTextRect.min.x, bodyTextRect.min.y + 50));

    iXY  area_size = iXY(
            31 * 8 + 8,
            Surface::getFontHeight() + 4 + 8);
    resizeClientArea(area_size);

    Init();
} // end IPAddressView constructor

void IPAddressView::Init()
{
    szServer.init("", 256,29);
    addInputField(iXY(4, 6), &szServer, "", true, 255);
} // end IPAddressView::init

// doDraw
//---------------------------------------------------------------------------
void IPAddressView::doDraw(Surface &viewArea, Surface &clientArea)
{
    View::doDraw(viewArea, clientArea);
} // end IPAddressView::doDraw

