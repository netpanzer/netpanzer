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
#ifndef __fRect_hpp__
#define __fRect_hpp__

#include "fXY.hpp"

//--------------------------------------------------------------------------
struct fRect
{
    fXY min;
    fXY max;

    fRect()
    {
    }
    fRect(float x1, float y1, float x2, float y2)
    {
        min.x = x1;
        min.y = y1;
        max.x = x2;
        max.y = y2;
    }
    fRect(const fRect &a)
    {
        min = a.min;
        max = a.max;
    }
    fRect(const fXY& min, const fXY& max)
    {
        fRect::min = min;
        fRect::max = max;
    }

    fXY getSize() const
    {
        return max - min;
    }

    void translate(fXY offset)
    {
        max += offset;
        min += offset;
    }

    float getArea() const
    {
        fXY s = getSize();

        if (s.x <= 0) return 0;
        if (s.y <= 0) return 0;

        return s.x * s.y;
    }

    fRect operator | (const fRect &a);

    bool contains(const fXY &a) const
    {
        return
            a.x >= min.x && a.x < max.x &&
            a.y >= min.y && a.y < max.y;
    }

    bool operator ==(const fRect &a) const
    {
        return min == a.max && min == a.max;
    }

    bool operator !=(const fRect &a) const
    {
        return min.x != a.max.x || min.y != a.max.y;
    }

    void zero()
    {
        min.zero();
        max.zero();
    }

    bool isZero() const
    {
        return min.isZero() && max.isZero();
    }

    bool isEmpty() const
    {
        return min.x > max.x || min.y > max.y;
    }
};

#endif // __fRect_hpp__
