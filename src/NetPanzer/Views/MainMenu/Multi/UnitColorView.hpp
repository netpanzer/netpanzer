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
#ifndef __UnitColorView_hpp__
#define __UnitColorView_hpp__

#include "Views/Components/View.hpp"
#include "2D/Surface.hpp"
#include "2D/PackedSurface.hpp"


//---------------------------------------------------------------------------
class UnitColorView : public View
{
private:
    Surface       fuckingSurface;
    Surface       grassSurface;
    PackedSurface packedBody;
    PackedSurface packedTurret;

public:
    UnitColorView();

    virtual void doDraw(Surface &windowArea, Surface &clientArea);
    virtual void rMouseDown  (const iXY &pos);
    virtual void rMouseUp    (const iXY &downPos, const iXY &upPos);
    virtual void rMouseDrag  (const iXY &downPos, const iXY &prevPos, const iXY &newPos);
}; // end UnitColorView

#endif // end __UnitColorView_hpp__
