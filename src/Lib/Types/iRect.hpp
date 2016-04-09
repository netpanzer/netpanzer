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
#ifndef __iRect_hpp__
#define __iRect_hpp__

#include "iXY.hpp"

struct iRect
{
    iXY min;
    iXY max;

    inline iRect()
    {}

    inline iRect(int x1, int y1, int x2, int y2)
    {
        min.x = x1;
        min.y = y1;
        max.x = x2;
        max.y = y2;
    }

    inline iRect(const iRect &a)
    {
        min = a.min;
        max = a.max;
    }

    inline iRect(const iXY &min, const iXY &max)
    {
        iRect::min = min;
        iRect::max = max;
    }

    inline iXY getSize() const
    {
        return max - min;
    }

    inline int getSizeX() const
    {
        return max.x - min.x;
    }
    inline int getSizeY() const
    {
        return max.y - min.y;
    }

    inline void translate(iXY offset)
    {
        max += offset;
        min += offset;
    }

    bool isEmpty() const
    {
        return min.x >= max.x || min.y >= max.y;
    }

    inline int getArea() const
    {
        iXY s = getSize();

        if (s.x <= 0) return 0;
        if (s.y <= 0) return 0;

        return s.x * s.y;
    }

    iRect operator | (const iRect &a);

    inline bool contains(const iXY &a) const
    {
        return
            a.x >= min.x && a.x < max.x &&
            a.y >= min.y && a.y < max.y;
    }

    inline bool operator ==(const iRect &a)
    {
        return min == a.min && max == a.max;
    }

    inline bool operator !=(const iRect &a)
    {
        return min != a.min || max != a.max;
    }

    inline void zero()
    {
        min.x = min.y = max.x = max.y = 0;
    }

    inline bool clip( const iRect &a )
    {
        if ( (a.min.y >= max.y) || (a.max.y <= min.y)	||
             (a.min.x >= max.x) || (a.max.x <= min.x) )
            return true;

        return false;
    }
};

#endif // __iRect_hpp__
