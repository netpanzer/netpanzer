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


#include "SmolderParticleSystem2D.hpp"
#include "Util/TimerInterface.hpp"


SmolderParticleSystem2D::SmolderParticleSystem2D(	const fXYZ  &pos,
        const iRect &emitBounds,
        float        lifetime,
        float        waitMin,
        float        waitMax,
        PUFF_TYPE    particleType) : ParticleSystem2D(pos, isFarAway)
{
    waitTime = 0.0f;

    iRect bounds(emitBounds);

    // Make sure the the bounding box is not negative.
    assert(bounds.getSize().x >= 0);
    assert(bounds.getSize().y >= 0);

    SmolderParticleSystem2D::bounds = bounds;

    // If all 0's were entered, give the bounds a size of 1.
    if(bounds.getSize().x == 0) {
        bounds.max.x = 1;
        bounds.min.x = 0;
    }
    if(bounds.getSize().y == 0) {
        bounds.max.y = 1;
        bounds.min.y = 0;
    }

    SmolderParticleSystem2D::lifetime     = lifetime;
    SmolderParticleSystem2D::waitMin      = waitMin;
    SmolderParticleSystem2D::waitMax      = waitMax;
    SmolderParticleSystem2D::particleType = particleType;

} // end SmolderParticleSystem2D

// sim
//---------------------------------------------------------------------------
void SmolderParticleSystem2D::sim()
{
    if (age <= lifetime) {
        waitTime += TimerInterface::getTimeSlice();

        //if (waitTime > (waitMin + float(rand() % waitDiff) / 256.0f))
        if (waitTime > waitMin) {
            // Genereate a location somewhere in the bounds of the smolder system.
            fXYZ offset(pos);

            if (bounds.getSize().x > 0) {
                offset.x += rand() % bounds.getSize().x;
            }
            if (bounds.getSize().y > 0) {
                offset.z += rand() % bounds.getSize().y;
            }

            // Generate a new smolder particle.
            try {
                new PuffParticle2D(offset, particleType, 0.0f, 1.0f, smokeFPSMin, smokeFPSRand, smokeLayer, smokeShadowLayer, smokeWindScale, Particle2D::getFarAway(offset));
            } catch(...) {}

            waitTime = 0.0f;
        }
    } else {
        isAlive = false;
    }

    ParticleSystem2D::sim();

} // end sim

//// SmolderParticleSystem2D
////---------------------------------------------------------------------------
//void SmolderParticleSystem2D::draw(const Surface &dest, SpriteSorter &sorter)
//{
//
//} // end SmolderParticleSystem2D
