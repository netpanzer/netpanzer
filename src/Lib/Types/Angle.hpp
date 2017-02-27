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
#ifndef _ANGLE_HPP
#define _ANGLE_HPP

#include <math.h>

#include "Types/iXY.hpp"
#include "Util/Endian.hpp"

class Angle
{
protected:
    static double deg_const;
    static double pi_const;

public:
    double angle;

public:
    Angle(double nAngle = 0.0)
    {
        angle = nAngle;
    }

    Angle(long x, long y)
    {
        angle = (double) atan2( (double) -y, (double) x );
    }

    Angle(const iXY& vec)
    {
        set( vec );
    }

    inline void set( const iXY &vec  )
    {
        angle = (double) atan( ((double) -vec.y / (double) vec.x) );

        if ( angle > 0 ) {
            if ( vec.x < 0 ) {
                angle -= pi_const;
            }
        } else
            if ( angle < 0 ) {
                if ( ((vec.y) < 0) && (vec.x < 0) ) {
                    angle += pi_const;
                }
            } else if ( angle == 0 ) {
                if ( vec.x < 0 )
                    angle += pi_const;
            }
    }

    double DegreesFloat() const
    {
        double degrees;
        degrees = deg_const * angle;
        if ( degrees < 0 )
            degrees = 360 - degrees;
        return( degrees );
    }

    long DegreesInt() const
    {
        double degrees;
        degrees = deg_const * angle;
        if ( degrees < 0 )
            degrees = 360 + degrees;
        return( (unsigned long ) degrees );
    }

    long DegreesInt( unsigned long granularity ) const
    {
        double degrees;
        degrees = deg_const * angle;
        if ( degrees < 0 )
            degrees = 360 + degrees;

        degrees = degrees / (double) granularity;
        return( (unsigned long ) degrees );
    }
};

#ifdef MSVC
#pragma pack(1)
#endif

struct NetworkAngleInt
{
public:
    NetworkAngleInt()
    { }

private:
    Sint32 angle_int;
    Uint32 grain;
    Sint32 angle_limit;

    friend class AngleInt;
} __attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

class AngleInt
{
public:
    long angle_int;
    unsigned long grain;
    long angle_limit;

    AngleInt( long nAngle = 0, unsigned long granularity = 360 )
    {
        angle_int = nAngle;
        grain = granularity;
        angle_limit = (360 / grain);
    }

    AngleInt( long x, long y)
    {
        angle_int = Angle(x,y).DegreesInt();
    }

    AngleInt( iXY &vec )
    {
        angle_int = Angle(vec).DegreesInt();
    }

    inline void set( long nAngle, unsigned long granularity )
    {
        angle_int = nAngle;
        grain = granularity;
        angle_limit = (360 / grain);
    }

    inline long operator++( )
    {
        angle_int++;
        if (angle_int == angle_limit )
            angle_int = 0;
        return( angle_int );
    }

    inline long operator--( )
    {
        angle_int--;
        if (angle_int < 0 )
            angle_int = angle_limit - 1;
        return( angle_int );
    }

    inline long Degrees() const
    {
        return( angle_int * grain );
    }

    NetworkAngleInt getNetworkAngleInt() const
    {
        NetworkAngleInt netangle;

        netangle.angle_int = htol32(angle_int);
        netangle.grain = htol32(grain);
        netangle.angle_limit = htol32(angle_limit);

        return netangle;
    }

    void setFromNetworkAngleInt(const NetworkAngleInt& netangle)
    {
        angle_int = ltoh32(netangle.angle_int);
        grain = ltoh32(netangle.grain);
        angle_limit = ltoh32(netangle.angle_limit);
    }
};

#endif
