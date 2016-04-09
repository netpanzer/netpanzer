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
#include <stdlib.h>
#include "Util/Log.hpp"
#include "GroundExplosionParticleSystem2D.hpp"
#include "Physics.hpp"
#include "GroundExplosionParticle2D.hpp"

//---------------------------------------------------------------------------
GroundExplosionParticleSystem2D::GroundExplosionParticleSystem2D(fXYZ pos, iRect bounds, int maxParticleCount, int maxParticleSpeed) : ParticleSystem2D(pos)
{
    // Make sure the the bounding box is not negative.
    assert(bounds.getSize().x >= 0);
    assert(bounds.getSize().y >= 0);
    assert(maxParticleSpeed > 0);

    // If all 0's were entered, give the bounds a size of 1.
    if(bounds.getSize().x == 0) {
        bounds.max.x = 1;
        bounds.min.x = 0;
    }
    if(bounds.getSize().y == 0) {
        bounds.max.y = 1;
        bounds.min.y = 0;
    }

    assert(maxParticleCount >= 1);

    int particleCount = (rand() % maxParticleCount) + 1; // max maxParticleCount, min 1

    // Generate the explosion particles
    fXYZ offset;

    // Calculate the maximum height of any particle spawned from this explosion.
    //float midTime = float(maxParticleSpeed) / Physics::getGravity();
    //float maxParticleHeight = Physics::getVerticalPosition(maxParticleSpeed, deg2Rad(90), midTime);

    for (int i = 0; i < particleCount; i++) {
        // Set the particle somewhere in the bounds of the explosion.
        offset.x = pos.x + rand() % bounds.getSize().x;
        offset.y = pos.y;
        offset.z = pos.z + rand() % bounds.getSize().y;

        // These particles have a hardcoded waitMax and waitMin.  They are the last 2 variables
        // in the following statement.
        // FIXME
        //new GroundExplosionParticle2D(offset, maxParticleSpeed);
    }

    isAlive = false;

} // end GroundExplosionParticleSystem2D::GroundExplosionParticleSystem2D
