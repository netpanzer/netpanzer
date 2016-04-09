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
#ifndef __Physics_hpp__
#define __Physics_hpp__

#include "Types/fXYZ.hpp"

////////////////////////////////////////////////////////////////////////////
// Physics declarations.
////////////////////////////////////////////////////////////////////////////

class Physics
{
public:
    class Wind
    {
        friend class Physics;

    private:
        float speed;        // The speed if the wind, no direction.
        int   direction;    // 0 - 628 counterClockwise starting from East.
        fXYZ  velocity;     // Movement per unit time.
        fXYZ  displacement; // Movement in time elapsed.

        void sim();
        void reset();

    public:
        Wind()
        {
            reset();
        }

        void setVelocity(float speed, int direction);
        void setSpeed(float speed)
        {
            this->speed = speed; setVelocity(speed, direction);
        }
        void setDirection(int direction)
        {
            this->direction = direction; setVelocity(speed, direction);
        }
        void setDirectionN()
        {
            direction = 157; setVelocity(speed, direction);
        }
        void setDirectionNE()
        {}
        void setDirectionE()
        {
            direction = 0; setVelocity(speed, direction);
        }
        void setDirectionSE()
        {}
        void setDirectionS()
        {
            direction = 471; setVelocity(speed, direction);
        }
        void setDirectionSW()
        {}
        void setDirectionW()
        {
            direction = 314; setVelocity(speed, direction);
        }
        void setDirectionNW()
        {}

        fXYZ getDisplacement() const
        {
            return displacement;
        }
    };

    enum { DIRECTION_TABLE_COUNT = 360 };

protected:
    static float gravity;
    static fXY directionTable[DIRECTION_TABLE_COUNT];


public:
    Physics()
    {}

    static Wind wind;

    static void sim()
    {
        wind.sim();
    }

    static void init();

    static float getRandomDirectionX();
    static float getRandomDirectionZ();

    static float getHorizontalPosition(int initialVelocity, int trajectoryAngle, float time);
    static float getVerticalPosition(int initialVelocity, int trajectoryAngle, float time);

    static float getGravity()
    {
        return gravity;
    }

    static void setGravity(float gravity)
    {
        Physics::gravity = gravity;
    }

}
; // end Physics

#endif // end __Physics_hpp__
