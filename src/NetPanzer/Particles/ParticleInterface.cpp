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


#include "Particles/ParticleInterface.hpp"
#include "PuffParticle2D.hpp"
#include "SmolderParticleSystem2D.hpp"
#include "FlameParticle2D.hpp"
#include "Particles/FlashParticle2D.hpp"
#include "TemplateExplosionSystem.hpp"
//#include "GroundExplosionParticle2D.hpp"
//#include "GroundExplosionParticleSystem2D.hpp"
#include "CloudParticle2D.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/KeyboardInterface.hpp"
#include "Physics.hpp"
//#include "DirtKickParticle2D.hpp"
#include "SparkParticle2D.hpp"
#include "FireParticleSystem2D.hpp"
//#include "RadarPingParticle2D.hpp"
//#include "MovementDirtPuffParticle2D.hpp"
#include "VectorPuffParticle2D.hpp"
#include "Units/UnitTypes.hpp"
#include "Interfaces/WorldViewInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "ChunkTrajectoryParticle2D.hpp"
//#include "SmokingTrajectoryParticle2D.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "TemplateMuzzleSystem.hpp"
#include "Units/UnitGlobals.hpp"
#include "CraterParticle2D.hpp"
#include "Util/Exception.hpp"
#include "System/Sound.hpp"


std::vector<UnitParticleInfo> ParticleInterface::unitParticleInfo;
int ParticleInterface::unitBodyMaxArea                   = 0;
int ParticleInterface::gParticlesCanHaveSmoke            = 1;
int ParticleInterface::gSolidColorExplosionParticles     = 0;
int ParticleInterface::gTestSim                          = 0;
int ParticleInterface::gDrawExplosionParticleCount       = 0;
int ParticleInterface::gExplosionFlameFlashCullMissCount = 1;
int ParticleInterface::gExplosionFlameFlashCullHitCount  = 1;
int ParticleInterface::gMuzzleSystemCullMissCount        = 1;
int ParticleInterface::gMuzzleSystemCullHitCount         = 1;

std::vector<int> ParticleInterface::unitHitPointTable;
std::vector<int> ParticleInterface::unitAttackFactorTable;


//--------------------------------------------------------------------------
ParticleInterface::ParticleInterface()
{}

//--------------------------------------------------------------------------
void ParticleInterface::addDirtPuffParticle(const iXY&)
{
    //	addPuffParticle(worldPos, DIRT_PUFF, smolderMinScale, smolderRandScale, smolderMinFPS, smolderRandFPS, smolderLayer);
}

//--------------------------------------------------------------------------
void ParticleInterface::addDirtPuffSystem(const iXY& worldPos,
        const iRect& bounds)
{
    (void) worldPos;
    (void) bounds;
    //	addPuffSystem(worldPos, bounds, dirtPuffMaxParticleCount, DIRT_PUFF, smolderMinScale, smolderRandScale, smolderMinFPS, smolderRandFPS, smolderLayer);
}

// addUnitDamagePuffParticle
//--------------------------------------------------------------------------
void ParticleInterface::addUnitDamagePuffParticle(const UnitState &unitState)
{
    iXY pos(unitState.location);

    iXY size     = unitParticleInfo[unitState.unit_type].minBounds.getSize();
    iXY halfSize = size / 2;

    fXYZ newPos;

    newPos.x = pos.x + (rand() % size.x) - halfSize.x;
    newPos.y = 0;
    newPos.z = pos.y + (rand() % size.y) - halfSize.y;

    int particleType = rand() % 2;

    if (particleType == 0) {
        try {
            new PuffParticle2D(	newPos,
                                LIGHT,
                                0.0f,
                                0.4f,
                                smokeFPSMin,
                                smokeFPSRand,
                                smokeLayer,
                                smokeShadowLayer,
                                smokeWindScale,
                                Particle2D::getFarAway(newPos));
        } catch(...) {
            // ignored for now
        }
    } else if (particleType == 1) {
        try {
            new PuffParticle2D(	newPos,
                                DARK,
                                0.0f,
                                0.4f,
                                smokeFPSMin,
                                smokeFPSRand,
                                smokeLayer,
                                smokeShadowLayer,
                                smokeWindScale,
                                Particle2D::getFarAway(newPos));
        } catch(...) {
            // ignored for now
        }
    } else {
        throw Exception("Unknonw DamagePuffParticle type");
    }
} // end ParticleInterface::addUnitDamagePuffParticle

//--------------------------------------------------------------------------
void ParticleInterface::addSmokePuffParticle(const iXY &worldPos)
{
    int particleType = rand() % 2;

    if (particleType == 0) {
        addSmokePuffParticle(worldPos, LIGHT);
    } else if (particleType == 1) {
        addSmokePuffParticle(worldPos, DARK);

    } else {
        assert(false);
    }

}

//--------------------------------------------------------------------------
void ParticleInterface::addSmokePuffParticle(const iXY &worldPos, float minScale, float randScale, int minFPS, int randFPS, int layer, float windScale /* = 1.0f */, float incrementScale /* = 0.0f */)
{
    int particleType = rand() % 2;

    if (particleType == 0) {
        addPuffParticle(worldPos, LIGHT, minScale, randScale, minFPS, randFPS, layer, windScale, incrementScale);
    } else if (particleType == 1) {
        addPuffParticle(worldPos, DARK, minScale, randScale, minFPS, randFPS, layer, windScale, incrementScale);

    } else {
        assert(false);
    }

}

//--------------------------------------------------------------------------
void ParticleInterface::addSmokePuffParticle(const iXY& worldPos,
        PUFF_TYPE type)
{
    (void) worldPos;
    (void) type;
    //	addPuffParticle(worldPos, type, smolderMinScale, smolderRandScale, smolderMinFPS, smolderRandFPS, smolderLayer, smolderWindScale, smolderIncrementScale);
}

//--------------------------------------------------------------------------
void ParticleInterface::addPuffParticle(const iXY &worldPos, PUFF_TYPE type, float minScale, float randScale, int minFPS, int randFPS, int layer, float windScale /* = 1.0f */, float incrementScale /* = 0.0f */)
{
    (void) worldPos;
    (void) type;
    (void) minScale;
    (void) randScale;
    (void) minFPS;
    (void) randFPS;
    (void) layer;
    (void) windScale;
    (void) incrementScale;
    try {
        //	new PuffParticle2D(fXYZ(worldPos.x, 0, worldPos.y), type, minScale, randScale, minFPS, randFPS, layer, layer - 1, windScale);
    } catch(...) {}
}

//--------------------------------------------------------------------------
void ParticleInterface::addSmokePuffSystem(const iXY &worldPos, const iRect &bounds, int maxParticleCount)
{
    (void) worldPos;
    (void) bounds;
    (void) maxParticleCount;
    //    int particleType = rand() % 2;
    //
    //    if      (particleType == 0)
    //    {
    //		addPuffSystem(worldPos, bounds, maxParticleCount, LIGHT, smolderMinScale, smolderRandScale, smolderMinFPS, smolderRandFPS, smolderLayer);
    //    }
    //    else if (particleType == 1)
    //    {
    //		addPuffSystem(worldPos, bounds, maxParticleCount, DARK, smolderMinScale, smolderRandScale, smolderMinFPS, smolderRandFPS, smolderLayer);
    //    }
}

//--------------------------------------------------------------------------
void ParticleInterface::addPuffSystem(const iXY &worldPos, const iRect &bounds, int maxParticleCount, PUFF_TYPE particleType, float minScale, float randScale, int minFPS, int randFPS, int layer)
{
    (void) worldPos;
    (void) bounds;
    (void) maxParticleCount;
    (void) particleType;
    (void) minScale;
    (void) randScale;
    (void) minFPS;
    (void) randFPS;
    (void) layer;
    //    // Make sure the the bounding box is not negative.
    //    assert(bounds.getSize().x >= 0);
    //    assert(bounds.getSize().y >= 0);
    //
    //	iRect r(bounds);
    //
    //    // If all 0's were entered, give the bounds a size of 1.
    //    if(r.getSize().x == 0)
    //    {
    //        r.max.x = 1;
    //        r.min.x = 0;
    //    }
    //    if(r.getSize().y == 0)
    //    {
    //        r.max.y = 1;
    //        r.min.y = 0;
    //    }
    //
    //    assert(maxParticleCount >= 1);
    //
    //    int particleCount = (rand() % maxParticleCount) + 1; // max maxParticleCount, min 1
    //
    //    // Generate the explosion particles
    //    fXYZ offset;
    //
    //    for (int i = 0; i < particleCount; i++)
    //    {
    //        // Set the particle somewhere in the bounds of the explosion.
    //        offset.x = worldPos.x + rand() % r.getSize().x;
    //        offset.y = 0;
    //        offset.z = worldPos.y + rand() % r.getSize().y;
    //
    //        // These particles have a hardcoded waitMax and waitMin.  They are the last 2 variables
    //        // in the following statement.
    //		  try {
    //        new PuffParticle2D(offset, particleType, minScale, randScale, minFPS, randFPS, layer, layer - 1, smolderWindScale);
    //		  } catch(...) {
    //        }
    //    }
}

//--------------------------------------------------------------------------
void ParticleInterface::addVehicleExplosionFlameParticle(const iXY &worldPos)
{
    (void) worldPos;
    //	addExplosionFlameParticle(worldPos, vehicleExplosionFlameMinScale, vehicleExplosionFlameRandScale, vehicleExplosionFlameLayer);
}

//--------------------------------------------------------------------------
void ParticleInterface::addVehicleExplosionFlashParticle(const iXY &worldPos)
{
    (void) worldPos;
    //	addFlashParticle(worldPos, vehicleExplosionFlashScaleMin, vehicleExplosionFlashScaleRand, vehicleExplosionFlashLifetime, vehicleExplosionFlashLayer);
}

//--------------------------------------------------------------------------
void ParticleInterface::addMissleExplosionFlameParticle(const iXY &worldPos)
{
    (void) worldPos;
    //	addExplosionFlameParticle(worldPos, missleExplosionFlameMinScale, missleExplosionFlameRandScale, missleExplosionFlameLayer);
}

//--------------------------------------------------------------------------
void ParticleInterface::addMissleExplosionFlashParticle(const iXY &worldPos)
{
    (void) worldPos;
    //	addFlashParticle(worldPos, missleExplosionFlashScaleMin, missleExplosionFlashScaleRand, missleExplosionFlashLifetime, missleExplosionFlashLayer);
}

//--------------------------------------------------------------------------
void ParticleInterface::addFlashParticle(const iXY &worldPos, float minScale, float randScale, float lifetime, int layer, bool singleFrame /* = false */)
{
    try {
        new FlashParticle2D(fXYZ(worldPos.x, 0, worldPos.y), minScale, randScale, lifetime, layer, singleFrame);
    } catch(...) {
        // ignrored
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::addExplosionFlameParticle(const iXY &worldPos, const float &minScale, const float &randScale, const int &layer)
{
    (void) worldPos;
    (void) minScale;
    (void) randScale;
    (void) layer;
    try {
        //	new ExplosionFireParticle2D(fXYZ(worldPos.x, 0, worldPos.y), minScale, randScale, layer);
    } catch(...) {
        // ignored
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::addMissleBurnParticle(const iXY &worldPos)
{
    (void) worldPos;
    //	addFlashParticle(worldPos, missleExplosionFlashScaleMin, missleExplosionFlashScaleRand, missleExplosionFlashLifetime, missleExplosionFlashLayer);
}

//--------------------------------------------------------------------------
void ParticleInterface::addExplosionSmokeParticle(const iXY &worldPos, int maxParticleSpeed)
{
    (void) worldPos;
    (void) maxParticleSpeed;
    // Chooses light or dark smoke.
    int particleType = rand() % 2;

    //iRect r(0, 0, 0, 0);

    if      (particleType == 0) {
        try {
            //new ExplosionParticleSystem2D(pos, r, 1, maxParticleSpeed, LIGHT);
        } catch(...) {
            // ignored
        }
    } else if (particleType == 1) {
        try {
            //new ExplosionParticleSystem2D(pos, r, 1, maxParticleSpeed, DARK);
        } catch(...) {
            // ignored
        }
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::addExplosionDirtSystem(const iXY &worldPos, const iRect &bounds)
{
    (void) worldPos;
    (void) bounds;
    //fXYZ  pos(worldPos.x, 0, worldPos.y);

    try {
        //new ExplosionParticleSystem2D(pos, bounds, dirtExplosionMaxParticleCount / 2, float(explosionParticleMaxSpeed) * 0.75f, DIRT_PUFF);
    } catch(...) {}
}

//--------------------------------------------------------------------------
void ParticleInterface::addGroundExplosionSystem(const iXY &worldPos, const iRect &bounds, int maxParticleCount, int maxParticleSpeed)
{
    (void) worldPos;
    (void) bounds;
    (void) maxParticleCount;
    (void) maxParticleSpeed;
    //fXYZ  pos(worldPos.x, 0, worldPos.y);

    try {
        //new GroundExplosionParticleSystem2D(pos, bounds, maxParticleCount, maxParticleSpeed);
    } catch(...) {}
}

//--------------------------------------------------------------------------
void ParticleInterface::addExplosionSmokeSystem(const iXY &worldPos, const iRect &bounds, int maxParticleCount, int maxParticleSpeed)
{
    (void) worldPos;
    (void) bounds;
    (void) maxParticleCount;
    (void) maxParticleSpeed;
    // Chooses light or dark smoke.
    /*
    int particleNum = rand() % 2;

    PUFF_TYPE particleType;

    if (particleNum == 0) {
        particleType = LIGHT;
    } else if (particleNum == 1) {
        particleType = DARK;
    }
    */
    //if (particleNum == 2) { particleType = DIRT_PUFF; }

    //fXYZ  pos(worldPos.x, 0, worldPos.y);

    try {
        //new ExplosionParticleSystem2D(pos, bounds, maxParticleCount, maxParticleSpeed, particleType);
    } catch(...) {}

    // Add an additional set of particles in the center of the larger explosion which
    // move at a slower initialVelocity, since real explosions have a lot of smoke lingering
    // in the middle of the smoke cloud.
    int slowerParticleSpeed = maxParticleSpeed - 1;

    if (slowerParticleSpeed > 0) {
        try {
            //new ExplosionParticleSystem2D(pos, bounds, maxParticleCount / 2, slowerParticleSpeed, particleType);
        } catch(...) {}
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::addSmolderSystem(const iXY &worldPos, const iRect &bounds, float lifetime, float percent)
{
    assert(percent > 0.0 && percent <= 100.0);

    float waitMin = ((100 - percent) / 100.0) * 1.0;
    float waitMax = ((100 - percent) / 100.0) * 0.3;

    // Chooses light or dark smoke.
    int particleType = rand() % 2;

    fXYZ  pos(worldPos.x, 0, worldPos.y);

    if      (particleType == 0) {
        try {
            new SmolderParticleSystem2D(pos, bounds, lifetime, waitMin, waitMax, LIGHT);
        } catch(...) {}
    }
    else if (particleType == 1) {
        try {
            new SmolderParticleSystem2D(pos, bounds, lifetime, waitMin, waitMax, DARK);
        } catch(...) {}
    }
    else {
        assert(false);
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::addCloudParticle(const iXY &worldPos, const iXY &worldSize)
{
    (void) worldSize;

    iXY shit;
    MapInterface::getMapPointSize(&shit);

    fXYZ  pos(worldPos.x, 0, worldPos.y);

    try {
        new CloudParticle2D(pos, iXY(shit.x, shit.y), cloudWindMinPercent, cloudWindRandPercent);
    } catch(...) {}
}

//--------------------------------------------------------------------------
void ParticleInterface::addCloudParticleSystem(int maxParticleCount)
{
    iXY shit;
    MapInterface::getMapPointSize(&shit);

    for (int i = 0; i < maxParticleCount; i++) {
        addCloudParticle(iXY(rand() % shit.x, rand() % shit.y), iXY(shit.x, shit.y));
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::testSim()
{
    static float groupTime0 = 0.0f;
    static float groupTime1 = 0.0f;

    groupTime0 += TimerInterface::getTimeSlice();
    groupTime1 += TimerInterface::getTimeSlice();

    if (groupTime0 > 1.0f) {
        groupTime0 = 0.0f;

        //SFX
        sound->playSound("expl");

        iRect gameViewRect;
        WorldViewInterface::getViewWindow(&gameViewRect);

        UnitState unitState;

        unitState.location.x = gameViewRect.min.x + 100 + (rand() % (gameViewRect.getSizeX() - 200));
        unitState.location.y = gameViewRect.min.y + 100 + (rand() % (gameViewRect.getSizeY() - 200));

        // Hack until all the units are actually used.
        unitState.unit_type = rand() % UnitProfileInterface::getNumUnitTypes();
        //unitState.unit_type = rand() % 7;

        addHit(unitState);
    }

    if (groupTime1 > 0.5f) {
        groupTime1 = 0.0f;

        //SFX
        sound->playSound("expl");

        iRect gameViewRect;
        WorldViewInterface::getViewWindow(&gameViewRect);

        iXY location;

        location.x = gameViewRect.min.x + 100 + (rand() % (gameViewRect.getSizeX() - 200));
        location.y = gameViewRect.min.y + 100 + (rand() % (gameViewRect.getSizeY() - 200));

        addMiss(location, rand() % UnitProfileInterface::getNumUnitTypes());
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::testSimText(Surface &dest)
{
    char strBuf[256];

    int y       = 50;
    int yOffset = 10;

    sprintf(strBuf, "particleSystemCount: %d", ParticleSystem2D::getParticleSystemCount());
    dest.bltString(2, y, strBuf, Color::white);
    y += yOffset;

    sprintf(strBuf, "particleCount:       %d", Particle2D::getFrameCount());
    dest.bltString(2, y, strBuf, Color::white);
}

//--------------------------------------------------------------------------
void ParticleInterface::addHit(const UnitState &unitState)
{
    ExplosionSystem e;

    float randomness = 1.0f; //Particle2D::getScale(0.8f, 0.4f);
    float frameRateAdjustment = getFrameRateAdjustment();

    e.particleCount = (int) (
                          8 * (frameRateAdjustment *
                               float(unitHitPointTable[unitState.unit_type]) * randomness));

    if (gDrawExplosionParticleCount)
    {
        ConsoleInterface::postMessage(Color::white, false, 0, "%d", e.particleCount);
    }

    float hitPointScale = 1.0f;

    UnitProfile *p = UnitProfileInterface::getUnitProfile(unitState.unit_type);
    if (p != 0) {
        hitPointScale = float(unitHitPointTable[unitState.unit_type]) / 18.0f;
    } else {
        assert(false);
    }

    e.percentScaleMin    = 0.8f;
    e.percentScaleRand   = 0.6f;
    e.speedSmall         = int(400 * hitPointScale);
    e.speedMedium        = int(250 * hitPointScale);
    e.speedLarge         = int(150 * hitPointScale);
    e.percentCountSmall  = 0.4f;
    e.percentCountMedium = 0.3f;
    e.percentCountLarge  = 0.3f;
    e.flameMinScale      = 0.2f * hitPointScale;
    e.flameRandScale     = 0.15f * hitPointScale;
    e.flameLifetime      = 0.1f;
    e.flameCount         = 1;
    e.flameBounds.zero();
    e.flashMinScale      = 0.6f * hitPointScale;
    e.flashRandScale     = 0.4f * hitPointScale;
    e.flashLifetime      = 0.5f;
    e.flashCount         = int( 3 * float(unitParticleInfo[unitState.unit_type].minBounds.getArea()) / float(unitBodyMaxArea));
    e.flashBounds        = unitParticleInfo[unitState.unit_type].minBounds;

    int randTrajectoryAngleMin = rand() % 3;

    if      (randTrajectoryAngleMin == 0) {
        e.minTrajectoryAngle = 86;
    } else if (randTrajectoryAngleMin == 1) {
        e.minTrajectoryAngle = 82;
    } else if (randTrajectoryAngleMin == 2) {
        e.minTrajectoryAngle = 77;
    }

    if (rand() % 2) {
        e.puffType = LIGHT;
    } else {
        e.puffType = DARK;
    }
    //explosionType.


    fXYZ pos(unitState.location.x, 0, unitState.location.y);

    try {
        new TemplateExplosionSystem(	pos,
                                     unitParticleInfo[unitState.unit_type].minBounds,
                                     e,
                                     Particle2D::getFarAway(pos));
    } catch(...) {}


    // Create some smoke in the middle of the explosion.
    //iXY   size(unitParticleInfo[unitState.unit_type].minBounds.getSize() / 2);
    //iRect bounds(-size, size);

    //addSmolderSystem(unitState.location, bounds, 3.0f, 75.0f);
    //	int num = rand() % 25;
    //
    //    // For fun, make a big explosion about every 25 explosions.
    //	float minScale  = 0.9f;
    //	float randScale = 0.5f;
    //
    //	float percentScale = (float(rand()) / float(RAND_MAX)) * randScale + minScale;
    //
    //	PUFF_TYPE puffType;
    //
    //	if (rand() % 2)
    //	{
    //		puffType = LIGHT;
    //	} else
    //	{
    //		puffType = DARK;
    //	}
    //
    //	new ExplosionParticleSystem2D(	fXYZ(worldPos.x, 0, worldPos.y),
    //									iRect(-5, -5, 5, 5),
    //									percentScale,
    //									puffType);
    //
    //	addVehicleExplosionFlameParticle(worldPos);
    //	addVehicleExplosionFlashParticle(worldPos);
    //
    //    num = rand() % 3;
    //
    //    if (num == 0)
    //    {
    //        num = rand() % 3;
    //
    //        if (num == 0)
    //        {
    //            int smolderLife    = rand() % 11 + 5;  // [5..15]
    //            int smolderPercent = rand() % 51 + 50; // [50..100]
    //
    //            //addSmolderSystem(worldPos, iRect(-5, -5, 5, 5), smolderLife, smolderPercent);
    //			//ConsoleInterface::postMessage("addSmolderSystem");
    //        }
    //    }
}

float ParticleInterface::getFrameRateAdjustment()
{
    static float frameMaxbase    = 60.0f;
    static float frameMinbase    = 20.0f;
    static float frameDifference = frameMaxbase - frameMinbase;

    if (TimerInterface::getFPS() > 60.0f) {
        return 1.0f;
    }

    float adjustment = (TimerInterface::getFPS() - frameMinbase) / frameDifference;

    if (adjustment < 0.2f) {
        adjustment = 0.2f;
    }

    return adjustment;
}

//--------------------------------------------------------------------------
void ParticleInterface::addMiss(const iXY &worldPos, Uint8 unitType)
{
    /*
      short hit_points;
      short attack_factor;
      long  attack_range;
      long  defend_range;
      char  speed_factor;
      char  speed_rate;
      char  reload_time;
      short regen_time;
    */

    ExplosionSystem e;

    float randomness = 1.0f; //Particle2D::getScale(0.8f, 0.4f);
    float frameRateAdjustment = getFrameRateAdjustment();

    e.particleCount = int(5 * (frameRateAdjustment * float(unitAttackFactorTable[unitType]) * randomness));

    if (gDrawExplosionParticleCount)
    {
       ConsoleInterface::postMessage(Color::white, false, 0, "%d", e.particleCount);
    }

    float attackScale = 1.0f;

    attackScale = float(unitAttackFactorTable[unitType]) / 15.0f;

    e.percentScaleMin    = 0.8f;
    e.percentScaleRand   = 0.4f;
    e.speedSmall         = int(400 * attackScale);
    e.speedMedium        = int(250 * attackScale);
    e.speedLarge         = int(150 * attackScale);
    e.percentCountSmall  = 0.5f;
    e.percentCountMedium = 0.3f;
    e.percentCountLarge  = 0.2f;
    e.flameMinScale      = 0.1f * attackScale;
    e.flameRandScale     = 0.1f * attackScale;
    e.flameLifetime      = 0.1f;
    e.flameCount         = 1;
    e.flameBounds.zero();
    e.flashMinScale      = 0.2f * attackScale;
    e.flashRandScale     = 0.3f * attackScale;
    e.flashLifetime      = 0.5f;
    e.flashCount         = 1;
    e.flashBounds.zero();

    int randTrajectoryAngleMin = rand() % 3;

    if      (randTrajectoryAngleMin == 0) {
        e.minTrajectoryAngle = 86;
    } else if (randTrajectoryAngleMin == 1) {
        e.minTrajectoryAngle = 82;
    } else if (randTrajectoryAngleMin == 2) {
        e.minTrajectoryAngle = 77;
    }

    e.puffType = DIRT;

    fXYZ pos(worldPos.x, 0, worldPos.y);

    try {
        new TemplateExplosionSystem( pos,
                                     iRect(-5, -5, 5, 5),
                                     e,
                                     Particle2D::getFarAway(pos));
    } catch(...) {}
}

void ParticleInterface::buildUnitTables()
{
    unitHitPointTable.resize(UnitProfileInterface::getNumUnitTypes());
    unitAttackFactorTable.resize(UnitProfileInterface::getNumUnitTypes());

    for (unsigned int i = 0; i < UnitProfileInterface::getNumUnitTypes(); i++) {
        UnitProfile *p = UnitProfileInterface::getUnitProfile(i);

        unitHitPointTable[i]     = int(sqrt(p->hit_points));
        unitAttackFactorTable[i] = int(sqrt(p->attack_factor * 2));
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::addMissleFlightPuff(const iXY &worldPos, const fXY &direction, float &curWait, float &totalWait, Uint8 unitType)
{
    (void) unitType;
    iXY thrustOffset(int(-10.0f * direction.x), int(-10.0f * direction.y));
    fXYZ loc(worldPos.x + thrustOffset.x, 0, worldPos.y + thrustOffset.y);

    curWait += TimerInterface::getTimeSlice();

    if (curWait > totalWait) {
        curWait   = 0.0f;
        totalWait = (float(rand()) / float(RAND_MAX)) * 0.03f + 0.02f;

        try {
            new PuffParticle2D(	loc,
                                LIGHT,
                                0.05f,
                                0.05f,
                                smokeFPSMin,
                                smokeFPSRand,
                                smokeLayer,
                                smokeShadowLayer,
                                MuzzleSystem::windScale,
                                Particle2D::getFarAway(loc));
        } catch(...) {
            // ignored
        }
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::addMissleLaunchPuff(const iXY &worldPos, const fXY &direction, Uint8 unitType)
{
    (void) worldPos;
    (void) direction;
    (void) unitType;
    //	//addMissleFlashParticle(pos);
    //
    //	//missleLaunchPuffWaitGroup += TimerInterface::getTimeSlice();
    //
    //	//if (missleLaunchPuffWaitGroup > missleLaunchPuffWaitTotal)
    //	//{
    //		//missleLaunchPuffWaitGroup = 0.0f;
    //		//missleLaunchPuffWaitTotal = MISSLE_FLIGHT_PUFF_RANDNUM;
    //	fXYZ pos(worldPos.x, 0, worldPos.y);
    //
    //	new VectorPuffParticle2D(pos, -direction, missleLaunchPuffMinScale, missleLaunchPuffRandScale, missleLaunchPuffSpeedMin, missleLaunchPuffSpeedRand, missleLaunchPuffWaitMin, missleLaunchPuffWaitRand, missleLaunchPuffLifetimeMin, missleLaunchPuffLifetimeRand, missleLaunchPuffWindScale);
    //	//for (int i = 0; i < 10; i++)
    //	//{
    //		//const int radius = 10;
    //
    //		//pos.x += (rand() % (radius << 1)) - radius;
    //		//pos.y += (rand() % (radius << 1)) - radius;
    //
    //		//addSmokePuffParticle(pos);
    //		//addSmokePuffParticle(pos, missleLaunchPuffMinScale, missleLaunchPuffRandScale, missleLaunchPuffMinFPS, missleLaunchPuffRandFPS, missleLaunchPuffLayer);
    //	//}
    //	//}
}

//--------------------------------------------------------------------------
void ParticleInterface::addMissleExplosion(const iXY &worldPos)
{
    addMissleExplosionFlameParticle(worldPos);
    addMissleExplosionFlashParticle(worldPos);
}

//--------------------------------------------------------------------------
void ParticleInterface::addMuzzlePuff(const fXYZ &worldPos, const fXYZ &direction, int frame, Uint8 unitType)
{
    // Cull out any muzzle puffs which are far away.
    if (Particle2D::getFarAway(worldPos)) {
        gMuzzleSystemCullHitCount++;
        return;
    } else {
        gMuzzleSystemCullMissCount++;
    }

    MuzzleSystem m;

    m.speedMin       = 100;
    m.speedRand      = 50;
    m.waitMin        = 0.05f;
    m.waitRand       = 0.05f;
    m.lifetimeMin    = 0.2f;
    m.lifetimeRand   = 0.1f;
    m.flashMinScale  = 0.3f;
    m.flashRandScale = 0.2f;
    m.puffMinScale   = 0.1f;
    m.puffRandScale  = 0.1f;
    m.puffType       = DARK;

// XXX CHECK
//    if (unitType == _unit_type_m109) {
//        m.muzzleType = MuzzleSystem::TRIPLE;
//    } else {
        m.muzzleType = MuzzleSystem::SINGLE;
//    }

    assert(frame < 36 && frame >= 0);
    fXYZ muzzlePos;

    //char strBuf[256];
    //sprintf(strBuf, "frame: %d", frame);
    //ConsoleInterface::postMessage(strBuf);

    iXY muzzleOffset(unitParticleInfo[unitType].muzzleTip[frame]);

    muzzlePos.x = worldPos.x + muzzleOffset.x;
    muzzlePos.z = worldPos.z + muzzleOffset.y;

    // Start the muzzle smoke.
    try {
        new TemplateMuzzleSystem(muzzlePos, direction, m);
    } catch(...) {}
}

//--------------------------------------------------------------------------
void ParticleInterface::addDirtKick(const iXY &worldPos)
{
    (void) worldPos;
    try {
        //new DirtKickParticle2D(fXYZ(worldPos.x, 0, worldPos.y));
    } catch(...) {}
}

//--------------------------------------------------------------------------
void ParticleInterface::addCloudParticle(int count /* = 1 */)
{
    if ( count < 0 )
    {
        count = 0;
    }

    iXY shit;
    MapInterface::getMapPointSize(&shit);

    for (int i = 0; i < count; i++) {
        try {
            new CloudParticle2D(fXYZ(rand() % shit.x, 0, rand() % shit.y), iXY(shit.x, shit.y), cloudWindMinPercent, cloudWindRandPercent);
        } catch(...) {}
    }
}

void ParticleInterface::initParticleSystems()
{
    PuffParticle2D::init();
    //SparkParticle2D::init();
    CloudParticle2D::init();
    FlameParticle2D::init();
    FlashParticle2D::init();
    //FireParticleSystem2D::initColors();
    //GroundExplosionParticle2D::init();
    ChunkTrajectoryParticle2D::init();
    CraterParticle2D::init();
    //DirtKickParticle2D::init();
    //RadarPingParticle2D::init();

    //iXY shit;
    //MapInterface::getMapPointSize(&shit);
    //addCloudParticle(iXY(400, 400), iXY(shit.x, shit.y));

    rebuildUnitParticleData();
}

void ParticleInterface::rebuildUnitParticleData()
{
    buildUnitTables();
    getUnitParticleInfo();
}

// Purpose: Add dirt puffs under the units.
//--------------------------------------------------------------------------
void ParticleInterface::addMoveDirtPuff(const UnitState &unitState)
{
    fXYZ pos(unitState.location.x, 0, unitState.location.y);

    int isFarAway = Particle2D::getFarAway(pos);

    // Remove some of the particles if they are far away.
    if (isFarAway) {
        if ((rand() % 3) == 1) {
            return;
        }
    }

    static float movePuffWaitGroup = 0.0f;
    static float movePuffWaitTotal = 0.0f;

    movePuffWaitGroup += TimerInterface::getTimeSlice();

    if (TileInterface::getWorldPixMovementValue(unitState.location.x, unitState.location.y) == 0) {
        if (movePuffWaitGroup >= movePuffWaitTotal) {
            iXY size     = unitParticleInfo[unitState.unit_type].minBounds.getSize();

            assert(size.x > 0 && size.y > 0);

            iXY halfSize = size / 2;

            pos.x += (rand() % size.x) - halfSize.x;
            pos.z += (rand() % size.y) - halfSize.y;

            movePuffWaitGroup = 0.0f;
            movePuffWaitTotal = (float(rand()) / float(RAND_MAX)) * 0.05f + 0.05f;

            try {
                new PuffParticle2D( pos,
                                    DIRT,
                                    0.0f,
                                    0.15f,
                                    (int) (smokeFPSMin * 0.75f),
                                    (int) (smokeFPSRand * 0.75f),
                                    1,
                                    0,
                                    MuzzleSystem::windScale,
                                    isFarAway);
            } catch(...) {
                // ignored for now
            }
        }
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::getUnitParticleInfo()
{
    // Create the correct number of unit information slots.
    unitParticleInfo.resize(UnitProfileInterface::getNumUnitTypes());

    for ( unsigned short i=0; i< UnitProfileInterface::getNumUnitTypes(); i++ )
    {
        unsigned short vector_index = i*GameManager::ststylesnum;
        UnitProfileSprites * uprofile = UnitProfileSprites::getUnitProfileSprites(vector_index);
        getMuzzleTips(uprofile->turretSprite, unitParticleInfo[i].muzzleTip);
        getMinBounds(uprofile->bodySprite, unitParticleInfo[i].minBounds);
    }
}

//--------------------------------------------------------------------------
void ParticleInterface::getMuzzleTips(PackedSurface &packedSource, iXY muzzleTips[36])
{
    // Copy all the packed images onto regular surfaces.
    Surface source(packedSource.getWidth(), packedSource.getHeight(), packedSource.getFrameCount());
    {
        for (int i = 0; i < packedSource.getFrameCount(); i++) {
            source.setFrame(i);
            source.fill(0);

            packedSource.setFrame(i);
            packedSource.blt(source, 0, 0);
        }
    }

    bool done = false;

    memset(muzzleTips, 0, sizeof(iXY) * 36);

    // Frame 0 - 8
    {for (int  i = 0; i < 9; i++)
        {
            source.setFrame(i);

            for (int offset = 0; offset < source.getCenterX(); offset++) {
                int yy = offset;
                for (int x = source.getWidth() - offset; x >= 0; x--) {
                    if (source.getPixel(x, yy) != 0) {
                        muzzleTips[i] = iXY(x, yy) - source.getCenter();
                        done = true;
                    }
                }

                if (done) {
                    break;
                }

                int xx = source.getWidth() - 1 - offset;
                for (unsigned int y = offset; y < source.getHeight(); y++) {
                    if (source.getPixel(xx, y) != 0) {
                        muzzleTips[i] = iXY(xx, y) - source.getCenter();
                        done = true;
                    }
                }

                if (done) {
                    break;
                }
            }

            done = false;
        }}

    done = false;

    // Frame 9 - 17
    {for (int i = 9; i < 18; i++)
        {
            source.setFrame(i);

            for (int offset = 0; offset < source.getCenterX(); offset++) {
                int yy = offset;
                for (unsigned int x = offset; x < source.getWidth(); x++) {
                    if (source.getPixel(x, yy) != 0) {
                        muzzleTips[i] = iXY(x, yy) - source.getCenter();
                        done = true;
                    }
                }

                if (done) {
                    break;
                }

                int xx = offset;
                for (unsigned int y = offset; y < source.getHeight(); y++) {
                    if (source.getPixel(xx, y) != 0) {
                        muzzleTips[i] = iXY(xx, y) - source.getCenter();
                        done = true;
                    }
                }

                if (done) {
                    break;
                }
            }

            done = false;
        }}

    done = false;

    // Frame 18 - 26
    {for (int i = 18; i < 27; i++)
        {
            source.setFrame(i);

            for (int offset = 0; offset < source.getCenterX(); offset++) {
                int yy = source.getHeight() - 1 - offset;
                for (int x = source.getWidth() - 1 - offset; x > 0; x--) {
                    if (source.getPixel(x, yy) != 0) {
                        muzzleTips[i] = iXY(x, yy) - source.getCenter();
                        done = true;
                    }
                }

                if (done) {
                    break;
                }

                int xx = offset;
                for (int y = source.getHeight() - 1; y >= 0; y--) {
                    if (source.getPixel(xx, y) != 0) {
                        muzzleTips[i] = iXY(xx, y) - source.getCenter();
                        done = true;
                    }
                }

                if (done) {
                    break;
                }
            }

            done = false;
        }}

    done = false;

    // Frame 27 - 35
    {for (int i = 27; i < 36; i++)
        {
            source.setFrame(i);

            for (unsigned int offset = 0; offset < (unsigned int)source.getCenterX(); offset++) {
                int yy = source.getHeight() - 1 - offset;
                for (unsigned int x = offset; x < source.getWidth(); x++) {
                    if (source.getPixel(x, yy) != 0) {
                        muzzleTips[i] = iXY(x, yy) - source.getCenter();
                        done = true;
                    }
                }

                if (done) {
                    break;
                }

                int xx = source.getWidth() - 1 - offset;
                for (int y = source.getHeight() - 1; y >= 0; y--) {
                    if (source.getPixel(xx, y) != 0) {
                        muzzleTips[i] = iXY(xx, y) - source.getCenter();
                        done = true;
                    }
                }

                if (done) {
                    break;
                }
            }

            done = false;
        }}

}

// getMinBounds
//--------------------------------------------------------------------------
void ParticleInterface::getMinBounds(PackedSurface &packedSource, iRect &minBounds)
{
    // Copy all the packed images onto regular surfaces.
    Surface source(packedSource.getWidth(), packedSource.getHeight(), packedSource.getFrameCount());
    for (int i = 0; i < packedSource.getFrameCount(); i++) {
        source.setFrame(i);
        source.fill(0);

        packedSource.setFrame(i);
        packedSource.blt(source, 0, 0);
    }

    int x;
    int y;

    // Get the minY.
    x = source.getCenterX();
    y = 0;
    source.setFrame(0);
    while (source.getPixel(x, y) == 0) {
        y++;
        minBounds.min.y = y;
    }

    // Get the maxY.
    x = source.getCenterX();
    y = source.getHeight() - 1;
    source.setFrame(0);
    while (source.getPixel(x, y) == 0) {
        y--;
        minBounds.max.y = y;
    }

    // Get the minX.
    x = 0;
    y = source.getCenterY();
    source.setFrame(9);
    while (source.getPixel(x, y) == 0) {
        x++;
        minBounds.min.x = x;
    }

    // Get the maxX.
    x = source.getWidth() - 1;
    y = source.getCenterY();
    source.setFrame(9);
    while (source.getPixel(x, y) == 0) {
        x--;
        minBounds.max.x = x;
    }

    if (minBounds.getArea() > unitBodyMaxArea) {
        unitBodyMaxArea = minBounds.getArea();
    }

} // end ParticleInterface::getMinBounds
