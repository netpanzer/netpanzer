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
#ifndef __fXYZ_hpp__
#define __fXYZ_hpp__

#include <math.h>

#include "fXY.hpp"

// fXYZ class declarations.
//---------------------------------------------------------------------------
struct fXYZ
{
    float x;
    float y;
    float z;

    static const fXYZ ZERO;

    fXYZ(float x=0, float y=0, float z=0)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    fXYZ(const fXYZ &a)
    {
        x = a.x;
        y = a.y;
        z = a.z;
    }

    fXYZ &operator =(const fXYZ &a)
    {
        x = a.x;
        y = a.y;
        z = a.z;
        return *this;
    }

    bool operator !=(const fXYZ &a)
    {
        if (x != a.x || x != a.y || x != a.z) return true;
        else return false;
    }

    fXYZ &operator +=(const fXYZ  &a)
    {
        x += a.x; y += a.y; z += a.z; return *this;
    }
    fXYZ &operator -=(const fXYZ  &a)
    {
        x -= a.x; y -= a.y; z -= a.z; return *this;
    }
    fXYZ &operator *=(      float  a)
    {
        x *=   a; y *=   a; z *=   a; return *this;
    }
    fXYZ &operator *=(const fXYZ  &a)
    {
        x *= a.x; y *= a.y; z *= a.z; return *this;
    }
    fXYZ &operator /=(      float  a)
    {
        x /=   a; y /=   a; z /=   a; return *this;
    }

    fXYZ  operator + (const fXYZ &a) const
    {
        return fXYZ(x + a.x, y + a.y, z + a.z);
    }
    fXYZ  operator - (const fXYZ &a) const
    {
        return fXYZ(x - a.x, y - a.y, z - a.z);
    }
    fXYZ  operator * (float a      ) const
    {
        return fXYZ(x *   a, y *   a, z *   a);
    }
    fXYZ  operator / (float a      ) const
    {
        return fXYZ(x /   a, y /   a, z /   a);
    }

    double  mag      () const
    {
        return sqrt(x * x + y * y + z * z);
    }
    double  mag2     () const
    {
        return x * x + y * y + z * z;
    }
    fXYZ getNormal() const
    {
        double a = 1.0 / sqrt(x * x + y * y + z * z);
        return fXYZ(x * a, y * a, z * a);
    }

    fXYZ &normalize()
    {
        return *this /= float(mag());
    }

    fXY project() const
    {
        if (z <= 1.0) return fXY(x, y);

        return fXY(x / z, y / z);
    }

    // Negation.
    fXYZ operator -() const
    {
        return fXYZ(-x, -y, -z);
    }

    void zero()
    {
        x = y = z = 0.0;
    }
}; // end fXYZ

static inline fXYZ cross(const fXYZ &a, const fXYZ &b)
{
    return fXYZ(
               a.y * b.z - b.y * a.z,
               a.z * b.x - b.z * a.x,
               a.x * b.y - b.x * a.y);
}

static inline float  dot(const fXYZ &a, const fXYZ &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline double distance (const fXYZ &a, const fXYZ &b)
{
    return (a - b).mag();
}

static inline double distance2(const fXYZ &a, const fXYZ &b)
{
    return (a - b).mag2();
}

//---------------------------------------------------------------------------
static inline fXYZ crossProduct(const fXYZ &a, const fXYZ &b)
{
    return fXYZ(
               (a.y * b.z) - (a.z * b.y),
               (a.z * b.x) - (a.x * b.z),
               (a.x * b.y) - (a.y * b.x)
           );
}

#endif // __fXYZ_hpp__
