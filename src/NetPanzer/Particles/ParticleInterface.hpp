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
#ifndef __ParticleInterface_hpp__
#define __ParticleInterface_hpp__

#include <vector>

#include "Types/iXY.hpp"
#include "Types/iRect.hpp"
#include "PuffParticle2D.hpp"
#include "ParticleSystemGlobals.hpp"
#include "Units/UnitTypes.hpp"
#include "Units/UnitState.hpp"

#include <vector>

class Surface;
class PackedSurface;

// Random waits for particles.
#define MISSLE_LAUNCH_PUFF_RANDNUM ((float(rand()) / float(RAND_MAX)) * 0.01f)

class UnitParticleInfo
{
public:
    iXY   muzzleTip[36];  // 36 frames per unit.
    iRect minBounds;      // Body min bounds.
}; // end UnitParticleInfo

// ParticleInterface
class ParticleInterface
{
private:
    static std::vector<UnitParticleInfo> unitParticleInfo;
    static int unitBodyMaxArea;

    static void getUnitParticleInfo();
    static void getMuzzleTips(PackedSurface &packedSource, iXY muzzleTips [36]);
    static void getMinBounds(PackedSurface &packedSource, iRect &minBounds);

    static std::vector<int>   unitHitPointTable;
    static std::vector<int>   unitAttackFactorTable;

    static void  buildUnitTables();
    static float getFrameRateAdjustment();

    static void addPuffSystem(const iXY &worldPos, const iRect &bounds, int maxParticleCount, PUFF_TYPE particleType, float minScale, float randScale, int minFPS, int randFPS, int layer);
    static void addGroundExplosionSystem(const iXY &worldPos, const iRect &bounds, int maxParticleCount, int maxParticleSpeed);

    static void addDirtPuffParticle(const iXY &worldPos);
    static void addDirtPuffSystem(const iXY &worldPos, const iRect &bounds);
    static void addExplosionDirtSystem(const iXY &worldPos, const iRect &bounds);

    static void addPuffParticle(const iXY &worldPos, PUFF_TYPE type, float minSize, float randSize, int minFPS, int randFPS, int layer, float windScale = 1.0f, float incrementScale = 0.0f);
    static void addSmokePuffParticle(const iXY &worldPos);
    static void addSmokePuffParticle(const iXY &worldPos, PUFF_TYPE type);
    static void addSmokePuffParticle(const iXY &worldPos, float minScale, float randScale, int minFPS, int randFPS, int layer, float windScale = 1.0f, float incrementScale = 0.0f);
    static void addSmokePuffSystem(const iXY &worldPos, const iRect &bounds, int maxParticleCount);

    static void addVehicleExplosionFlameParticle(const iXY &worldPos);
    static void addVehicleExplosionFlashParticle(const iXY &worldPos);

    static void addMissleExplosionFlameParticle(const iXY &worldPos);
    static void addMissleExplosionFlashParticle(const iXY &worldPos);

    static void addFlashParticle(const iXY &worldPos, float minScale, float randScale, float lifetime, int layer, bool singleFrame = false);
    static void addExplosionFlameParticle(const iXY &worldPos, const float &minScale, const float &randScale, const int &layer);

    static void addMissleBurnParticle(const iXY &worldPos);

    static void addExplosionSmokeParticle(const iXY &worldPos, int maxParticleSpeed);
    static void addExplosionSmokeSystem(const iXY &worldPos, const iRect &bounds, int maxParticleCount, int maxParticleSpeed);

    static void addSmolderSystem(const iXY &worldPos, const iRect &bounds, float lifetime, float percent);

    static void addCloudParticle(const iXY &worldPos, const iXY &worldSize);
    static void addCloudParticleSystem(int maxParticleCount);

    static void addMovementDirtPuffParticle(const iXY &worldPos);

public:
    static int gParticlesCanHaveSmoke;
    static int gSolidColorExplosionParticles;
    static int gTestSim;
    static int gDrawExplosionParticleCount;

    static int gExplosionFlameFlashCullMissCount;
    static int gExplosionFlameFlashCullHitCount;
    static int gMuzzleSystemCullMissCount;
    static int gMuzzleSystemCullHitCount;

    static int getExplosionFlameFlashCullMissCount()
    {
        return gExplosionFlameFlashCullMissCount;
    }
    static int getExplosionFlameFlashCullHitCount()
    {
        return gExplosionFlameFlashCullHitCount;
    }
    static int getMuzzleSystemCullMissCount()
    {
        return gMuzzleSystemCullMissCount;
    }
    static int getMuzzleSystemCullHitCount()
    {
        return gMuzzleSystemCullHitCount;
    }

    ParticleInterface();

    static void testSim();
    static void testSimText(Surface &dest);

    static void addHit(const UnitState &unitState);
    static void addMiss(const iXY &worldPos, Uint8 unitType);

    static void addMissleLaunchPuff(const iXY &worldPos, const fXY &direction, Uint8 unitType);
    static void addMissleFlightPuff(const iXY &worldPos, const fXY &direction, float &curWait, float &totalWait, Uint8 unitType);
    static void addMissleExplosion(const iXY &worldPos);

    //static void addMuzzlePuff(const fXYZ &muzzlePos, const fXYZ &direction, Uint8 unitType);
    static void addMuzzlePuff(const fXYZ &worldPos, const fXYZ &direction, int frame, Uint8 unitType);
    static void addDirtKick(const iXY &worldPos);

    static void addCloudParticle(int count = 1);

    static void addMoveDirtPuff(const UnitState &unitState);

    static void initParticleSystems();
    static void rebuildUnitParticleData();

    static void addUnitDamagePuffParticle(const UnitState &unitState);
}; // end ParticleInterface

#endif // __ParticleInterface_hpp__
