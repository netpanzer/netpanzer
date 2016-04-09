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
#ifndef __iXY_hpp__
#define __iXY_hpp__

struct iXY
{
    int x;
    int y;

    iXY()
        : x(0), y(0)
    {
    }
        
    iXY(int newx, int newy)
        : x(newx), y(newy)
    {
    }

    iXY(const iXY &a)
    {
        x = a.x;
        y = a.y;
    }

    iXY &operator =(const iXY &a)
    {
        x = a.x;
        y = a.y;
        return *this;
    }

    iXY  &operator +=(const iXY &a)
    {
        x += a.x; y += a.y; return *this;
    }
    iXY  &operator -=(const iXY &a)
    {
        x -= a.x; y -= a.y; return *this;
    }
    iXY  &operator *=(const iXY &a)
    {
        x *= a.x; y *= a.y; return *this;
    }
    iXY  &operator *=(int        a)
    {
        x *=   a; y *=   a; return *this;
    }
    iXY	&operator /=(int        a)
    {
        x /=   a; y /=   a; return *this;
    }

    iXY operator + (const iXY &a) const
    {
        return iXY(x + a.x, y + a.y);
    }
    iXY operator - (const iXY &a) const
    {
        return iXY(x - a.x, y - a.y);
    }
    iXY operator * (const iXY &a) const
    {
        return iXY(x * a.x, y * a.y);
    }
    iXY operator * (int        a) const
    {
        return iXY(x *   a, y *   a);
    }
    iXY operator / (int        a) const
    {
        return iXY(x /   a, y /   a);
    }

    bool operator ==(const iXY &a) const
    {
        return x == a.x && y == a.y;
    }

    bool operator !=(const iXY &a) const
    {
        return x != a.x || y != a.y;
    }

    // Negation.
    iXY operator -() const
    {
        return iXY(-x, -y);
    }

    void zero()
    {
        x = y = 0;
    }

    bool isZero() const
    {
        return x==0 && y==0;
    }

    double mag() const;
    float  mag2() const
    {
        return float(x * x + y * y);
    }

    int getArea() const
    {
        return x * y;
    }
};

//---------------------------------------------------------------------------
static inline double distance(const iXY &a, const iXY &b)
{
    return (a - b).mag();
}

//---------------------------------------------------------------------------
static inline float distance2(const iXY &a, const iXY &b)
{
    return (a - b).mag2();
}

#endif // __iXY_hpp__
