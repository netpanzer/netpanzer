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


#include "VectorPuffParticle2D.hpp"
#include "PuffParticle2D.hpp"
#include "Util/TimerInterface.hpp"


// VectorPuffParticle2D
//---------------------------------------------------------------------------
VectorPuffParticle2D::VectorPuffParticle2D(
    const fXYZ  &pos,
    const fXYZ  &direction,
    float        scaleMin,
    float        scaleRand,
    float        speedMin,
    float        speedRand,
    float        waitMin,
    float        waitRand,
    float        lifetimeMin,
    float        lifetimeRand,
    float        windScale) : WindParticle2D(pos, windScale)
{
    VectorPuffParticle2D::scaleMin  = scaleMin;
    VectorPuffParticle2D::scaleRand = scaleRand;
    VectorPuffParticle2D::isFarAway = isFarAway;

    wait     = ((float(rand()) / float(RAND_MAX)) * waitRand + waitMin);
    velocity = direction * ((float(rand()) / float(RAND_MAX)) * speedRand + speedMin);
    lifetime = getLifetime(lifetimeMin, lifetimeRand);

    waitTime = 0.0f;

} // end VectorPuffParticle2D::VectorPuffParticle2D

// VectorPuffParticle2D::sim
//---------------------------------------------------------------------------
// Purpose: Handles the particle simulation, no drawing.
//---------------------------------------------------------------------------
void VectorPuffParticle2D::sim()
{
    assert(this != 0);

    float dt = TimerInterface::getTimeSlice();

    pos += velocity * dt;

    waitTime += dt;

    if (waitTime > wait) {
        int particleType = rand() % 2;

        if (particleType == 0) {
            try {
                new PuffParticle2D(	pos,
                                    LIGHT,
                                    scaleMin,
                                    scaleRand,
                                    smokeFPSMin,
                                    smokeFPSRand,
                                    smokeLayer,
                                    smokeShadowLayer,
                                    windScale,
                                    0);
            } catch(...) {}
        }
        else if (particleType == 1) {
            try {
                new PuffParticle2D(	pos,
                                    DARK,
                                    scaleMin,
                                    scaleRand,
                                    smokeFPSMin,
                                    smokeFPSRand,
                                    smokeLayer,
                                    smokeShadowLayer,
                                    windScale,
                                    0);
            } catch(...) {}
        }

        waitTime = 0.0f;

        try {
            //new cSparkParticle2D(fXYZ(pos.x, pos.y, zPos));
        } catch(...) {}
    }

    if (age > lifetime) {
        isAlive = false;
    }

    WindParticle2D::sim();

} // end VectorPuffParticle2D::sim
