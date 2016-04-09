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


#include "Util/Log.hpp"
#include "GroundExplosionParticle2D.hpp"
#include "2D/Color.hpp"

Surface GroundExplosionParticle2D::groundParticleSprite0;


// GroundExplosionParticle2D constructor
//---------------------------------------------------------------------------
GroundExplosionParticle2D::GroundExplosionParticle2D(const fXYZ& pos,
        int maxSpeed)
// XXX trajectoryAngle was missing here, I added 0
        : TrajectoryParticle2D(pos, maxSpeed, 0)
{
    airLifetime = lifetime;
    lifetime    = 10;

    // XXX
    // groundParticleSurface.setOffsetCenter();

} // end GroundExplosionParticle2D::GroundExplosionParticle2D

// init
void GroundExplosionParticle2D::init()
{
    groundParticleSprite0.create(4, 4, 1);
    groundParticleSprite0.fill(Color::brown);
    //groundParticleSprite0.loadTIL("pics/particles/puff/dirt/Smoke16.til");
    //groundParticleSprite0.shrinkWrap();
    //groundParticleSprite0.scale(6);

} // end GroundExplosionParticle2D::init

// GroundExplosionParticle2D::draw
//---------------------------------------------------------------------------
void GroundExplosionParticle2D::draw(const Surface &dest, SpriteSorter &sorter)
{
    (void) dest;
    (void) sorter;
    // Calculate some projection to get the 3D angle effect.  The greater the particleYScale,
    // the greater the particle bend in the air.
    //float zPos = pos.z;// - pos.y * particleYScale;

    // XXX
#if 0
    groundParticleSurface.setTo(groundParticleSprite0);

    groundParticleSurface.setAttrib(iXY(pos.x, zPos), 3);
    sorter.addSprite(&groundParticleSurface);
#endif
} // end GroundExplosionParticle2D::draw

/*
// GroundExplosionParticle2D::sim
//---------------------------------------------------------------------------
// Purpose: Handles the particle simulation of a trajectory particle, no drawing.
//---------------------------------------------------------------------------
void GroundExplosionParticle2D::sim()
{
if (age < airLifetime)
{
pos += velocity;
pos.y = physics.getVerticalPosition(initialVelocity, trajectoryAngle, age);
}

if (age >= lifetime)
{
isAlive = false;
}

cParticle2D::sim();
} // end GroundExplosionParticle2D::sim
*/
