/*
Copyright (C) 2011 by Aaron Perez <aaronps@gmail.com>

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

#ifndef GFLAGSELECTIONVIEW_HPP
#define GFLAGSELECTIONVIEW_HPP

#include "Core/CoreTypes.hpp"
#include "Views/Components/View.hpp"
#include "2D/Surface.hpp"

//---------------------------------------------------------------------------
class GFlagSelectionView : public View
{
private:
    iXY loc_player_flag;
    const char * text_current;
    iXY loc_text_current;
    bool loaded;

    iRect rect;

public:
    GFlagSelectionView();
    virtual ~GFlagSelectionView()
    {}

    virtual void doActivate();
    virtual void doDeactivate();
    virtual void doDraw(Surface &windowArea, Surface &clientArea);
    virtual void checkResolution(iXY oldResolution, iXY newResolution);
    virtual void processEvents();

    //virtual int  lMouseUp(const iXY &downPos, const iXY &upPos);
    //virtual void drawBorder(Surface& )
    //{}

    void init();
};

#endif // GFLAGSELECTIONVIEW_HPP
