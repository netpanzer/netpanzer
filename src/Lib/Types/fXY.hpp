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
#ifndef __fXY_hpp__
#define __fXY_hpp__

class fXY
{
public:
    float x, y;

    fXY()
        : x(0), y(0)
    {}
    fXY(float newx, float newy)
        : x(newx), y(newy)
    {
    }
    fXY(const fXY &a)
    {
        x = a.x;
        y = a.y;
    }

    fXY &operator =(const fXY &a)
    {
        x = a.x;
        y = a.y;
        return *this;
    }

    fXY &operator +=(const fXY &a)
    {
        x += a.x; y += a.y; return *this;
    }
    fXY &operator -=(const fXY &a)
    {
        x -= a.x; y -= a.y; return *this;
    }
    fXY &operator *=(const fXY &a)
    {
        x *= a.x; y *= a.y; return *this;
    }
    fXY &operator *=(float      a)
    {
        x *=   a; y *=   a; return *this;
    }
    fXY &operator /=(float      a)
    {
        x /=   a; y /=   a; return *this;
    }

    fXY operator + (const fXY &a) const
    {
        return fXY(x + a.x, y + a.y);
    }
    fXY operator - (const fXY &a) const
    {
        return fXY(x - a.x, y - a.y);
    }
    fXY operator * (const fXY &a) const
    {
        return fXY(x * a.x, y * a.y);
    }
    fXY operator * (float      a) const
    {
        return fXY(x *   a, y *   a);
    }
    fXY operator / (float      a) const
    {
        return fXY(x /   a, y /   a);
    }

    bool operator ==(const fXY &a) const
    {
        return x == a.x && y == a.y;
    }

    bool operator !=(const fXY &a) const
    {
        return x != a.x || y != a.y;
    }

    // Negation.
    fXY operator -() const
    {
        return fXY(-x, -y);
    }

    void zero()
    {
        x = y = 0.0f;
    }

    bool isZero() const
    {
        return x==0 && y==0;
    }

    double mag() const;
    float  mag2() const
    {
        return x * x + y * y ;
    }

    double getArea() const
    {
        return x * y;
    }
};

//---------------------------------------------------------------------------
inline double distance(const fXY &a, const fXY &b)
{
    return (a - b).mag();
}

//---------------------------------------------------------------------------
inline float distance2(const fXY &a, const fXY &b)
{
    return (a - b).mag2();
}

#endif // __fXY_hpp__
