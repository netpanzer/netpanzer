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


#include "cButton.hpp"
#include "2D/Surface.hpp"
#include "Util/Exception.hpp"
#include "2D/Color.hpp"
#include "ViewGlobals.hpp"


void cButton::createPacked(const iXY &pos, PackedSurface &source, const char *toolTip, ITEM_FUNC leftClickFunc)
{
    Surface tempTopSurface(source.getWidth(), source.getHeight(), source.getFrameCount());

    for (unsigned int i = 0; i < tempTopSurface.getNumFrames(); i++) {
        tempTopSurface.setFrame(i);
        tempTopSurface.fill(0);
        source.setFrame(i);
        source.blt(tempTopSurface, 0, 0);
    }

    topSurface.pack(tempTopSurface);

    cButton::toolTip       = strdup(toolTip); assert(toolTip != 0);
    cButton::bounds        = iRect(pos.x, pos.y, pos.x + tempTopSurface.getWidth(), pos.y + tempTopSurface.getHeight());
    cButton::leftClickFunc = leftClickFunc;
}

// createCenterText
//---------------------------------------------------------------------------
// Purpose: Creates a button with the name centered in the absolute horizontal
//          middle of the button.
//---------------------------------------------------------------------------
void cButton::createCenterText(iXY pos,
                               int xSize,
                               const char *nName,
                               const char *nToolTip,
                               ITEM_FUNC nLeftClickFunc)
{
    Surface tempTopSurface;

    int ySize = Surface::getFontHeight();
    tempTopSurface.create(xSize, ySize, 3);

    // Find out the horizontal offset to put the button name on the button.

    // Make the unselected button
    tempTopSurface.fill(componentBodyColor);
    tempTopSurface.drawButtonBorder(topLeftBorderColor, bottomRightBorderColor);
    tempTopSurface.bltStringCenter(nName, componentInActiveTextColor);

    // Make the mouse-over button
    tempTopSurface.setFrame(1);
    tempTopSurface.fill(componentBodyColor);
    tempTopSurface.drawButtonBorder(topLeftBorderColor, bottomRightBorderColor);
    tempTopSurface.bltStringCenter(nName, componentFocusTextColor);

    // Make the selected button
    tempTopSurface.setFrame(2);
    tempTopSurface.fill(componentBodyColor);
    tempTopSurface.drawButtonBorder(bottomRightBorderColor, topLeftBorderColor);
    tempTopSurface.bltStringCenter(nName, componentActiveTextColor);

    // Save the button name.
    setName(nName); assert(name != 0);

    // Save the button tool tip.
    toolTip = strdup(nToolTip); assert(toolTip != 0);

    // Save the bounds of the button.
    bounds = iRect(pos.x, pos.y, pos.x+tempTopSurface.getWidth(), pos.y+tempTopSurface.getHeight());

    // Save the function associated with the button.
    leftClickFunc = nLeftClickFunc;

    // Reset the button to unselected status.
    tempTopSurface.setFrame(0);

    topSurface.pack(tempTopSurface);

} // end createCenterText

// RESET
//---------------------------------------------------------------------------
void cButton::reset()
{
    toolTip        = 0;
    name           = 0;
    isSelected     = false;
    leftClickFunc  = 0;
    rightClickFunc = 0;
} // end RESET

// SET NAME
//---------------------------------------------------------------------------
void cButton::setName(const char *buttonName)
{
    name = strdup(buttonName);
    if (buttonName == 0) throw Exception("ERROR: Unable to allocate button name: %s", buttonName);
} // end SET NAME

