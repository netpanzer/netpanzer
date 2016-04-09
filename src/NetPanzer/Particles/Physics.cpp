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


#include <string.h>
#include <stdlib.h>
#include "Physics.hpp"
#include "Util/TimerInterface.hpp"
#include "Util/Math.hpp"

Physics::Wind  Physics::wind;

float Physics::gravity;
fXY  Physics::directionTable[DIRECTION_TABLE_COUNT];

//--------------------------------------------------------------------------
void Physics::init()
{
    // Clear out the velocity table.
    memset(&directionTable, 0, sizeof(fXYZ));

    gravity = 32 * 12;

    float radians;

    for (int degree = 0; degree < DIRECTION_TABLE_COUNT; degree++) {
        radians = Math::deg2Rad(degree);

        fXY direction;
        direction.x = float(cos(radians));
        //direction.y = float(Math::deg2Rad((rand() % 20) + 70)); // Radians
        //const int min = 80;
        //direction.y = (rand() % (90 - min)) + min; // Degrees
        direction.y = float(sin(radians));

        directionTable[degree] = direction;
    }
}

//--------------------------------------------------------------------------
void Physics::Wind::sim()
{
    displacement = velocity * TimerInterface::getTimeSlice();
}

//--------------------------------------------------------------------------
void Physics::Wind::reset()
{
    speed        = 0;
    direction    = 0;
    velocity     = fXYZ(0.0f, 0.0f, 0.0f);
    displacement = fXYZ(0.0f, 0.0f, 0.0f);
}

//--------------------------------------------------------------------------
void Physics::Wind::setVelocity(float speed, int direction)
{
    //float radians = Math::deg2Rad(direction);

    velocity.x =  speed * float(Math::getCos(direction));
    velocity.y = 0;
    velocity.z = -speed * float(Math::getSin(direction));
}

//--------------------------------------------------------------------------
float Physics::getRandomDirectionX()
{
    return directionTable[rand() % DIRECTION_TABLE_COUNT].x;
}

//--------------------------------------------------------------------------
float Physics::getRandomDirectionZ()
{
    return directionTable[rand() % DIRECTION_TABLE_COUNT].y;
}

//--------------------------------------------------------------------------
float Physics::getHorizontalPosition(int initialVelocity, int trajectoryAngle, float time)
{
    // !NOTE! Make sure the trajectoryAngle is in radians.
    return (initialVelocity * Math::getCos(trajectoryAngle)) * time;
}

//--------------------------------------------------------------------------
float Physics::getVerticalPosition(int initialVelocity, int trajectoryAngle, float time)
{
    // !NOTE! Make sure the trajectoryAngle is in radians.
    return (initialVelocity * Math::getSin(trajectoryAngle)) * time - (getGravity() / 2.0f) * (time * time);
}
