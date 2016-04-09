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
#ifndef __ParticleSystemGlobals_hpp__
#define __ParticleSystemGlobals_hpp__


#if _MSC_VER > 1000
	#pragma once
#endif


#include "Types/iRect.hpp"


enum
{
    BLT_TO_SURFACE,
    BLT_TO_SPRITE_SORTER
};

enum PUFF_TYPE
{
    LIGHT,
    DARK,
    SMOKE,
    DIRT
};

enum EXPLOSION_TYPE
{
    GROUND,
    PLAYER,
    NONPLAYER
};

// All Particles.
//--------------------------------------------------------------------------
extern float arcScale;
extern float shadowDistanceScale;

// Cloud Particle.
//--------------------------------------------------------------------------
extern int   cloudLayer;
extern int   cloudShadowLayer;
extern float cloudWindMinPercent;
extern float cloudWindRandPercent;
extern float speedAbsoluteMin;

// Cloud Coverage.
//--------------------------------------------------------------------------
extern int   baseTileCountPerCloud;
extern float randomDivisorOfBase;
extern float clearPercentOfBase;
extern float brokenPercentOfBase;
extern float partlyCloudyPercentOfBase;
extern float overcastPercentOfBase;
extern float extremelyCloudyPercentOfBase;

// Wind Speeds.
//--------------------------------------------------------------------------
extern int   baseWindSpeed;
extern float calmWindsPercentOfBase;
extern float breezyWindsPercentOfBase;
extern float briskWindsPercentOfBase;
extern float heavyWindsPercentOfBase;
extern float typhoonWindsPercentOfBase;

// Smoke Particles.
//--------------------------------------------------------------------------
extern int   smokeLayer;
extern int   smokeShadowLayer;
extern int   smokeFPSMin;
extern int   smokeFPSRand;
extern float smokeWindScale;

//--------------------------------------------------------------------------
class MuzzleSystem
{
public:
    enum MUZZLE_TYPE
    {
        SINGLE,
        TRIPLE
    };

    int    speedMin;
    int    speedRand;
    float  waitMin;
    float  waitRand;
    float  lifetimeMin;
    float  lifetimeRand;
    float  flashMinScale;
    float  flashRandScale;
    float  puffMinScale;
    float  puffRandScale;
    char  *name;

    PUFF_TYPE   puffType;
    MUZZLE_TYPE muzzleType;

    static float windScale;
    static int   layer;
    static int   shadowLayer;
    static int   flashLayer;

}
; // end MuzzleSystem

//--------------------------------------------------------------------------
class ExplosionSystem
{
public:
    int    particleCount;
    float  percentScaleMin;
    float  percentScaleRand;
    int    speedSmall;
    int    speedMedium;
    int    speedLarge;
    float  percentCountSmall;
    float  percentCountMedium;
    float  percentCountLarge;
    float  flameMinScale;
    float  flameRandScale;
    float  flameLifetime;
    int    flameCount;
    iRect  flameBounds;
    float  flashMinScale;
    float  flashRandScale;
    float  flashLifetime;
    int    flashCount;
    iRect  flashBounds;
    int    minTrajectoryAngle;
    char  *name;

    PUFF_TYPE      puffType;
    EXPLOSION_TYPE explosionType;

    static float puffSmallScaleMin;
    static float puffSmallScaleRand;
    static float puffMediumScaleMin;
    static float puffMediumScaleRand;
    static float puffLargeScaleMin;
    static float puffLargeScaleRand;

    static float puffSmallWaitMin;
    static float puffSmallWaitRand;
    static float puffMediumWaitMin;
    static float puffMediumWaitRand;
    static float puffLargeWaitMin;
    static float puffLargeWaitRand;

    static int flameLayer;
    static int flashLayer;

}
; // end ExplosionSystem

//--------------------------------------------------------------------------
class VehicleExplosionSystem : public ExplosionSystem
    {}
;

#endif // __ParticleSystemGlobals_hpp__
