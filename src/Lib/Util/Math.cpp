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


#include <assert.h>
#include "Math.hpp"


float Math::cosTable[360 + 1];
float Math::sinTable[360 + 1];


// rad2Deg
//--------------------------------------------------------------------------
// Purpose: Converts from radians to degrees.
//--------------------------------------------------------------------------
int Math::rad2Deg(float radian)
{
    return int(radian * (180.0 / M_PI));

} // end Math::rad2Deg

// deg2Rad
//--------------------------------------------------------------------------
// Purpose: Converts from degrees to radians.
//--------------------------------------------------------------------------
float Math::deg2Rad(int degree)
{
    return degree * (M_PI / 180.0);

} // end Math::deg2Rad

// degAngle
//--------------------------------------------------------------------------
int Math::degAngle(const iXY &a, const iXY &b)
{
    return rad2Deg(radAngle(a, b));

} // end Math::degAngle

// degAngle
//--------------------------------------------------------------------------
int Math::degAngle(float x, float y)
{
    return rad2Deg(radAngle(x, y));

} // end Math::degAngle

// unitDirection
//--------------------------------------------------------------------------
fXY Math::unitDirection(const iXY &a, const iXY &b)
{
    float angle = radAngle(a, b);

    return unitDirection(angle);

} // end Math::unitDirection

// unitDirection
//--------------------------------------------------------------------------
fXY Math::unitDirection(float angle)
{
    return fXY(cos(angle), sin(angle));

} // end Math::unitDirection

// radAngle
//--------------------------------------------------------------------------
float Math::radAngle(const iXY &a, const iXY &b)
{
    return atan2(b.y - a.y, b.x - a.x);

} // end Math::radAngle

// radAngle
//--------------------------------------------------------------------------
float Math::radAngle(float x, float y)
{
    return atan2(y, x);

} // end Math::radAngle

// unitDirectionEast
//--------------------------------------------------------------------------
fXY Math::unitDirectionEast(const fXY &northDirection)
{
    float angle = radAngle(northDirection.x, northDirection.y);

    angle += M_PI / 2.0;

    return unitDirection(angle);

} // end Math::unitDirectionEast

// unitDirectionWest
//--------------------------------------------------------------------------
fXY Math::unitDirectionWest(const fXY &northDirection)
{
    float angle = radAngle(northDirection.x, northDirection.y);

    angle -= M_PI / 2.0;

    return unitDirection(angle);

} // end Math::unitDirectionWest

// getCos
//--------------------------------------------------------------------------
float Math::getCos(int angle)
{
    assert(angle >= 0 && angle <= 360);

    return cosTable[angle];

} // end Math::getCos

// getSin
//--------------------------------------------------------------------------
float Math::getSin(int angle)
{
    assert(angle >= 0 && angle <= 360);

    return sinTable[angle];

} // end Math::getSin

// init
//--------------------------------------------------------------------------
Math::Math()
{
    for (int i = 0; i <= 360; i++) {
        float radians = deg2Rad(i);

        cosTable[i] = cos(radians);
        sinTable[i] = sin(radians);
    }
} // end Math::init

Math math;
