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


#include "TrajectoryParticle2D.hpp"
#include "Physics.hpp"
#include "Util/Math.hpp"


//---------------------------------------------------------------------------
TrajectoryParticle2D::TrajectoryParticle2D(	const fXYZ &pos,
        int         maxSpeed,
        float       trajectoryAngle,
        int         dieAtMidFlight /* = 0 */) : Particle2D(pos)
{
    initialVelocity                       = rand() % maxSpeed;
    direction.x                           = Physics::getRandomDirectionX();
    direction.z                           = Physics::getRandomDirectionZ();
    TrajectoryParticle2D::trajectoryAngle = trajectoryAngle;
    initialPos                            = pos;

    TrajectoryParticle2D::dieAtMidFlight = dieAtMidFlight;

    lifetime = (float(initialVelocity * Math::getSin(int(trajectoryAngle))) / Physics::getGravity()) * 2.0f;

    if (dieAtMidFlight) {
        halfLife = lifetime / 2.0f;
    }

    // Calculate the lifetime of this particle based off of the angle of trajectory
    // and the initialVelocity of the particle.

    // This makes the particle die when it reaches its peak height.
    //lifetime = (float(initialVelocity * sin(trajectoryAngle)) / Physics::getGravity());

    //velocity.x = direction.x * initialVelocity;
    //velocity.y = 0;
    //velocity.z = direction.z * initialVelocity;

} // end TrajectoryParticle2D::TrajectoryParticle2D

// sim
//---------------------------------------------------------------------------
// Purpose: Handles the particle simulation of a trajectory particle, no drawing.
//---------------------------------------------------------------------------
void TrajectoryParticle2D::sim()
{
    // Get the particle horizontal position.
    float dx = Physics::getHorizontalPosition(initialVelocity, int(trajectoryAngle), age);

    //pos += velocity;

    // Move the particle in the correct direction based off the direction the particle
    // is facing and the horizontal placement the particle has experienced.
    pos.x = initialPos.x + dx * direction.x;
    pos.z = initialPos.z + dx * direction.z;

    pos.y = Physics::getVerticalPosition(initialVelocity, int(trajectoryAngle), age);

    if (dieAtMidFlight) {
        if (age >= halfLife) {
            isAlive = false;
        }
    } else if (age >= lifetime) {
        isAlive = false;
    }

    Particle2D::sim();

} // end TrajectoryParticle2D::sim
