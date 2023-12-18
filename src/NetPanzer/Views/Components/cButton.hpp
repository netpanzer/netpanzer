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
#ifndef __cButton_hpp__
#define __cButton_hpp__

#include <string.h>
#include <stdlib.h>
#include "Types/iXY.hpp"
#include "Types/iRect.hpp"
#include "2D/PackedSurface.hpp"
#include "Util/NoCopy.hpp"

class Surface;

class cButton : public NoCopy
{
public:
    cButton()
    {
        reset();
    }
    ~cButton()
    {
        if (name    != 0) free(name);
        if (toolTip != 0) free(toolTip);
    }

    typedef void (*ITEM_FUNC)(void);

    ITEM_FUNC rightClickFunc;
    ITEM_FUNC leftClickFunc;
    PackedSurface topSurface;
    PackedSurface bottomSurface;

    iRect getBounds    () const
    {
        return bounds;
    }
    const char *getName () const
    {
        return name;
    }
    bool  getIsSelected() const
    {
        return isSelected;
    }
    const char* getToolTip() const
    {
        return toolTip;
    }

    void createPacked(const iXY &pos, PackedSurface &source, const char *toolTip, ITEM_FUNC leftClickFunc);

    void createCenterText(iXY pos, int xSize, const char *nName, const char *nToolTip, ITEM_FUNC nLeftClickFunc);

    inline bool contains(iXY pos)
    {
        assert(this != 0);

        if (	pos.x >= bounds.min.x &&
                pos.y >=  bounds.min.y &&
                pos.x < bounds.max.x &&
                pos.y < bounds.max.y) {
            return true;

        } else {
            return false;
        }
    }
    bool   isSelected;

private:
    iRect  bounds;
    char  *toolTip;
    char  *name;

    void  reset();

protected:
    void setName(const char *buttonName);

}
; // end cButton

#endif // end __cButton_hpp__
