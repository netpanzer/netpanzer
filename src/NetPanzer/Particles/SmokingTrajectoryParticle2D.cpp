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


#include "SmokingTrajectoryParticle2D.hpp"
#include "Util/TimerInterface.hpp"


//---------------------------------------------------------------------------
SmokingTrajectoryParticle2D::SmokingTrajectoryParticle2D(	const fXYZ &pos,
        int         maxSpeed,
        float       waitMin,
        float       waitRand,
        PUFF_PARTICLE_TYPE particleType)
// XXX had to add an angle here
        : TrajectoryParticle2D(pos, maxSpeed, 0)
{
    assert(this != 0);

    SmokingTrajectoryParticle2D::waitMin  = waitMin;
    SmokingTrajectoryParticle2D::waitRand = waitRand;
    SmokingTrajectoryParticle2D::waitTime = 0.0f;

    SmokingTrajectoryParticle2D::particleType = particleType;

} // end SmokingTrajectoryParticle2D

// SmokingTrajectoryParticle2D::sim
//---------------------------------------------------------------------------
// Purpose: Handles the particle simulation, no drawing.
//---------------------------------------------------------------------------
void SmokingTrajectoryParticle2D::sim()
{
    assert(this != 0);
    assert(false);

    //	waitTime += TimerInterface::getTimeSlice();
    //
    //	// Calculate some projection to get the 3D angle effect.  The greater the explosionParticleArc,
    //	// the greater the particle bend in the air.
    //	float zPos = pos.z - pos.y * explosionParticleArc;
    //
    //	// The following variable is so that as the images get larger (higher), they
    //	// are not drawn as often since they mostly overlay anyways.
    //	float heightWait = pos.y / 256.0f;
    //
    //#define EXPLOSION_PARTICLE_RANDNUM (((float(rand()) / float(RAND_MAX)) - 0.25f))
    //
    //	if (waitTime > waitMin + EXPLOSION_PARTICLE_RANDNUM + heightWait)
    //	//if (waitTime > RANDNUM + heightWait)
    //	//if (waitTime > waitMin + RANDNUM)
    //	{
    //		// Add a new puff of smoke at the current position of each particle.
    //		iXY randOffset((rand() % 20) - 20, (rand() % 20) - 20);
    //		pos.x += randOffset.x;
    //		zPos  += randOffset.y;
    //
    //		// This is a fudge to make the particle get bigger the higher they are in the air.
    //		const float deltaY = 100.0f;
    //
    //		if      (particleType == LIGHT_SMOKE_PUFF)
    //		{
    //			new PuffParticle2D(fXYZ(pos.x, pos.y, zPos), LIGHT_SMOKE_PUFF, pos.y / deltaY, 0.2f, smolderMinFPS, smolderRandFPS, smolderLayer, smolderWindScale);
    //		}
    //		else if (particleType == DARK_SMOKE_PUFF)
    //		{
    //			new PuffParticle2D(fXYZ(pos.x, pos.y, zPos), DARK_SMOKE_PUFF, pos.y / deltaY, 0.2f, smolderMinFPS, smolderRandFPS, smolderLayer, smolderWindScale);
    //		}
    //		else if (particleType == DIRT_PUFF)
    //		{
    //			new PuffParticle2D(fXYZ(pos.x, pos.y, zPos), DIRT_PUFF, pos.y / deltaY, 0.2f, smolderMinFPS, smolderRandFPS, smolderLayer, smolderWindScale);
    //		}
    //
    //		waitTime = 0.0f;
    //	}
    //
    //	TrajectoryParticle2D::sim();
    //
} // end SmokingTrajectoryParticle2D::sim
