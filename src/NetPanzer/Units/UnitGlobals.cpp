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

#include "UnitGlobals.hpp"
#include "2D/Palette.hpp"
#include "2D/Surface.hpp"

int unitLayer = 3;

void colorMapGray(PackedSurface &source, PackedSurface &dest, ColorTable &colorTable)
{
    //colormapping gray scheme.
    Surface tempSurface;

    tempSurface.create(source.getWidth(),source.getHeight(), source.getFrameCount());

    for (int i = 0; i < source.getFrameCount(); i++) {
        tempSurface.setFrame(i);
        tempSurface.fill(Color::white);
        source.setFrame(i);
        source.blt(tempSurface, 0, 0);
        tempSurface.bltLookup(iRect( 0, 0,
                        tempSurface.getWidth(), tempSurface.getHeight() ),
                        colorTable.getColorArray());
    }

    dest.pack(tempSurface);
}
