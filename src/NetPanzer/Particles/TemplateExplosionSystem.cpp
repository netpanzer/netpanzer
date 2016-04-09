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


#include "TemplateExplosionSystem.hpp"
#include "ChunkTrajectoryParticle2D.hpp"
#include "FlameParticle2D.hpp"
#include "Particles/FlashParticle2D.hpp"
#include "Particles/Particle2D.hpp"
#include "Util/TimerInterface.hpp"
#include "CraterParticle2D.hpp"
#include "Interfaces/TileInterface.hpp"
#include "Particles/ParticleInterface.hpp"


// TemplateExplosionSystem
//---------------------------------------------------------------------------
TemplateExplosionSystem::TemplateExplosionSystem(const fXYZ& pos,
        const iRect           &emitBounds,
        const ExplosionSystem &e,
        int                    isFarAway) : ParticleSystem2D(pos, isFarAway)
{
    // Make sure the the bounding box is not negative.
    assert(emitBounds.getSizeX() >= 0);
    assert(emitBounds.getSizeY() >= 0);

    iRect bounds(emitBounds);

    // If all 0's were entered, give the bounds a size of 1.
    if (bounds.getSizeX() == 0) {
        bounds.max.x = 1;
        bounds.min.x = 0;
    }
    if (bounds.getSizeY() == 0) {
        bounds.max.y = 1;
        bounds.min.y = 0;
    }

    int halfBoundsX = bounds.getSizeX() / 2;
    int halfBoundsY = bounds.getSizeY() / 2;

    // Add a crater on the ground if it is a logical location.
    int pixMovementValue = TileInterface::getWorldPixMovementValue(int(pos.x), int(pos.z));

    // Check for water or impassible.
    if (pixMovementValue != 5 &&
            pixMovementValue != 4) {
        try {
            new CraterParticle2D(pos);
        } catch(...) {}
    }

    // If this is water, we can not have smoke.
    int canHaveSmoke = 1;

    // Get the number of particles for this system.
    int particleCount = 0;

    // Reduce particle count if far away.
    if (isFarAway) {
        particleCount = e.particleCount / 3;
    } else if (pixMovementValue == 5) {
        // Since this is a water explosion and only particles are getting
        // thrown, add some more particles.
        canHaveSmoke  = 0;
        particleCount = int(e.particleCount * 1.7f);
    } else {
        // Since this is a normal explosion leave the particle count alone.
        particleCount = e.particleCount;
    }

    // Only flash and explosion flame if it is near the screen.
    if (!isFarAway && canHaveSmoke) {
        ParticleInterface::gExplosionFlameFlashCullMissCount++;

        // Add the flame(s).
        try {
            new FlameParticle2D(pos, e.flameMinScale, e.flameRandScale, e.flameLifetime, ExplosionSystem::flameLayer);
        } catch(...) {}

        // Add the flash(s).
        //if (e.flashCount <= 1)
        //{
        try {
            new FlashParticle2D(pos, e.flashMinScale, e.flashRandScale, e.flashLifetime, ExplosionSystem::flashLayer);
        } catch(...) {}
        //}

        //for (int i = 0; i < e.flashCount - 1; i++)
        //{
        //	fXYZ offset;
        //
        //	offset.x = pos.x - (rand() % bounds.getSizeX()) + halfBoundsX;
        //	offset.y = 0.0f;
        //	offset.z = pos.z - (rand() % bounds.getSizeY()) + halfBoundsY;
        //
        //	new FlashParticle2D(offset, e.flashMinScale, e.flashRandScale, e.flashLifetime, ExplosionSystem::flashLayer);
        //}
    }
    else {
        ParticleInterface::gExplosionFlameFlashCullHitCount++;
    }

    // This is a randomness for each explosion.
    float percentScale = Particle2D::getScale(e.percentScaleMin, e.percentScaleRand);

    // Get the number of particles of each type.
    int smallCount  = int(particleCount * e.percentCountSmall);
    int mediumCount = int(particleCount * e.percentCountMedium);
    int largeCount  = int(particleCount * e.percentCountLarge);

    // Check to see if we need to make some adjustments for low frame rates.
    int fpsLow = (TimerInterface::getFPS() < 20) ? 1 : 0;

    // Small smoke.
    float maxSpeed  = percentScale * e.speedSmall;
    float scaleMin  = ExplosionSystem::puffSmallScaleMin;
    float scaleRand = ExplosionSystem::puffSmallScaleRand;
    float waitMin   = percentScale * ExplosionSystem::puffSmallWaitMin;
    float waitRand  = percentScale * ExplosionSystem::puffSmallWaitRand;

    fXYZ offset;

    {
        for (int i = 0; i < smallCount; i++) {
            offset.x = pos.x - (rand() % bounds.getSizeX()) + halfBoundsX;
            offset.y = pos.y;
            offset.z = pos.z - (rand() % bounds.getSizeY()) + halfBoundsY;

            try {
                new ChunkTrajectoryParticle2D(	offset,
                                               int(maxSpeed),
                                               scaleMin,
                                               scaleRand,
                                               waitMin,
                                               waitRand,
                                               e.minTrajectoryAngle,
                                               e.puffType,
                                               (rand() % 2) ? true : false,
                                               isFarAway || fpsLow,
                                               canHaveSmoke);
            } catch(...) {}
        }
    }

    // Medium smoke.
    maxSpeed  = percentScale * e.speedMedium;
    scaleMin  = ExplosionSystem::puffMediumScaleMin;
    scaleRand = ExplosionSystem::puffMediumScaleRand;
    waitMin   = percentScale * ExplosionSystem::puffMediumWaitMin;
    waitRand  = percentScale * ExplosionSystem::puffMediumWaitRand;

    {
        for (int i = 0; i < mediumCount; i++) {
            offset.x = pos.x - (rand() % bounds.getSizeX()) + halfBoundsX;
            offset.y = pos.y;
            offset.z = pos.z - (rand() % bounds.getSizeY()) + halfBoundsY;

            try {
                new ChunkTrajectoryParticle2D(	pos,
                                               int(maxSpeed),
                                               scaleMin,
                                               scaleRand,
                                               waitMin,
                                               waitRand,
                                               e.minTrajectoryAngle,
                                               e.puffType,
                                               true,
                                               isFarAway || fpsLow,
                                               canHaveSmoke);
            } catch(...) {}
        }
    }

    // Large smoke.
    maxSpeed  = percentScale * e.speedLarge;
    scaleMin  = ExplosionSystem::puffLargeScaleMin;
    scaleRand = ExplosionSystem::puffLargeScaleRand;
    waitMin   = percentScale * ExplosionSystem::puffLargeWaitMin;
    waitRand  = percentScale * ExplosionSystem::puffLargeWaitRand;

    {
        for (int i = 0; i < largeCount; i++) {
            offset.x = pos.x - (rand() % bounds.getSizeX()) + halfBoundsX;
            offset.y = pos.y;
            offset.z = pos.z - (rand() % bounds.getSizeY()) + halfBoundsY;

            try {
                new ChunkTrajectoryParticle2D(	pos,
                                               int(maxSpeed),
                                               scaleMin,
                                               scaleRand,
                                               waitMin,
                                               waitRand,
                                               e.minTrajectoryAngle,
                                               e.puffType,
                                               true,
                                               isFarAway || fpsLow,
                                               canHaveSmoke);
            } catch(...) {}
        }
    }

    // Kill me after I release my particles.
    isAlive = false;

} // end TemplateExplosionSystem::TemplateExplosionSystem
