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

#ifndef __ChunkTrajectoryParticle2D_hpp__
#define __ChunkTrajectoryParticle2D_hpp__


#include "TrajectoryParticle2D.hpp"
#include "ParticleSystemGlobals.hpp"
#include "2D/PackedSurface.hpp"


// ChunkTrajectoryParticle2D
//--------------------------------------------------------------------------
class ChunkTrajectoryParticle2D : public TrajectoryParticle2D
{
protected:
    static PackedSurface staticPackedGroundChunks;
    static PackedSurface staticPackedBurnGroundChunks;
    static PackedSurface staticPackedUnitGreenChunks;
    static PackedSurface staticPackedUnitGrayChunks;

    static void createPAKFiles();
    static void createGroundChunks();
    static void createBurnGroundChunks();
    static void createUnitBodyGreenChunks();
    static void createUnitBodyGrayChunks();

    float     arcYPix;
    int       hasSmoke;
    float     smokeWaitTime;
    float     smokeCurWait;
    float     scaleMin;
    float     scaleRand;
    float     waitMin;
    float     waitRand;
    PUFF_TYPE particleType;

public:
    ChunkTrajectoryParticle2D(	const fXYZ &pos,
                               int         maxSpeed,
                               float       scaleMin,
                               float       scaleRand,
                               float       waitMin,
                               float       waitRand,
                               int         minTrajectoryAngle,
                               PUFF_TYPE   particleType,
                               int         dieAtMidFlight = 0,
                               int         isFarAway      = 0,
                               int         canHaveSmoke   = 1);

    virtual void sim();
    virtual void draw(const Surface &dest, SpriteSorter &sorter);

    static void init();
    static void uninit();
}
; // end ChunkTrajectoryParticle2D

#endif // __ChunkTrajectoryParticle2D_hpp__
