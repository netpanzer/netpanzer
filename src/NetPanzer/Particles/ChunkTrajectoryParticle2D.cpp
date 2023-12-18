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


#include "ChunkTrajectoryParticle2D.hpp"
#include "Util/TimerInterface.hpp"
#include "PuffParticle2D.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Util/Math.hpp"
#include "Interfaces/TileInterface.hpp"
#include "Units/UnitGlobals.hpp"
#include "Particles/ParticleInterface.hpp"
#include "Units/UnitProfileInterface.hpp"


PackedSurface ChunkTrajectoryParticle2D::staticPackedGroundChunks;
PackedSurface ChunkTrajectoryParticle2D::staticPackedBurnGroundChunks;
PackedSurface ChunkTrajectoryParticle2D::staticPackedUnitGreenChunks;
PackedSurface ChunkTrajectoryParticle2D::staticPackedUnitGrayChunks;


// ChunkTrajectoryParticle2D
//---------------------------------------------------------------------------
ChunkTrajectoryParticle2D::ChunkTrajectoryParticle2D(	const fXYZ &pos,
        int         maxSpeed,
        float       scaleMin,
        float       scaleRand,
        float       waitMin,
        float       waitRand,
        int         minTrajectoryAngle,
        PUFF_TYPE   particleType,
        int         dieAtMidFlight /* = 0 */,
        int         isFarAway      /* = 0 */,
        int         canHaveSmoke   /* = 1 */) : TrajectoryParticle2D(pos, maxSpeed, (rand() % (90 - minTrajectoryAngle)) + minTrajectoryAngle, dieAtMidFlight)
{
    assert(this != 0);
    assert(minTrajectoryAngle >= 0 && minTrajectoryAngle <= 90);

    ChunkTrajectoryParticle2D::isFarAway    = isFarAway;

    ChunkTrajectoryParticle2D::scaleMin     = scaleMin;
    ChunkTrajectoryParticle2D::scaleRand    = scaleRand;

    ChunkTrajectoryParticle2D::waitMin      = waitMin;
    ChunkTrajectoryParticle2D::waitRand     = waitRand;

    if (isFarAway) {
        // Decrease the amount of smoke on particle.
        ChunkTrajectoryParticle2D::waitMin  *= 2.0f;
        ChunkTrajectoryParticle2D::waitRand *= 2.0f;
    }

    ChunkTrajectoryParticle2D::particleType = particleType;

    //TileInterface::getWorldPixColor(int worldX, int worldY)

    // int randChunk = rand() % staticPackedGroundChunks.getFrameCount();

    packedSurface.setData(staticPackedGroundChunks);
    packedSurface.setDrawModeSolid();
    //packedSurface.setDrawModeBlend(&Palette::colorTableBrighten);

    // Get particle color.
    if (ParticleInterface::gSolidColorExplosionParticles) {
        index = 253;

    } else {
        index = TileInterface::getWorldPixColor((int) pos.x, (int) pos.z);
    }

    //int randFrame = rand() % staticPackedChunks[randChunk].getFrameCount();
    staticPackedGroundChunks.setFrame(index);

    packedSurfaceShadow.setData(staticPackedGroundChunks);
    packedSurfaceShadow.setDrawModeBlend(&Palette::colorTableDarkenALittle);

    if (ParticleInterface::gParticlesCanHaveSmoke && canHaveSmoke) {
        hasSmoke = rand() % 3;
    } else {
        hasSmoke = 0;
    }

    smokeCurWait  = 0.0f;
    smokeWaitTime = 0.0f;
    arcYPix       = 0.0f;

} // end ChunkTrajectoryParticle2D::ChunkTrajectoryParticle2D

// init
//---------------------------------------------------------------------------
void ChunkTrajectoryParticle2D::init()
{
    createPAKFiles();

} // end ChunkTrajectoryParticle2D::init

void ChunkTrajectoryParticle2D::uninit()
{
    staticPackedGroundChunks.unload("pics/particles/chunks/pak/groundChunks.pak");
    staticPackedBurnGroundChunks.unload("pics/particles/chunks/pak/greenUnitChunks.pak");
    staticPackedUnitGreenChunks.unload("pics/particles/chunks/pak/grayUnitChunks.pak");
    staticPackedUnitGrayChunks.unload("pics/particles/chunks/pak/burnGroundChunks.pak");
}



// draw
//---------------------------------------------------------------------------
void ChunkTrajectoryParticle2D::draw(const Surface& , SpriteSorter &sorter)
{
    assert(this != 0);

    packedSurface.setAttrib(iXY((int) pos.x, (int) (pos.z - arcYPix)), layer);
    sorter.addSprite(&packedSurface);

    if (GameConfig::video_shadows) {
        packedSurfaceShadow.setAttrib(iXY((int) (pos.x - arcYPix), (int) pos.z), shadowLayer);
        sorter.addSprite(&packedSurfaceShadow);
    }

} // end ChunkTrajectoryParticle2D::draw

// ChunkTrajectoryParticle2D::sim
//---------------------------------------------------------------------------
// Purpose: Handles the particle simulation, no drawing.
//---------------------------------------------------------------------------
void ChunkTrajectoryParticle2D::sim()
{
    assert(this != 0);

    arcYPix = pos.y * arcScale;

    if (hasSmoke == 1) {
        smokeCurWait += TimerInterface::getTimeSlice();

        if (smokeCurWait > smokeWaitTime) {
            try {
                new PuffParticle2D(fXYZ(pos.x, pos.y, pos.z - arcYPix), fXYZ(pos.x - arcYPix, pos.y, pos.z), particleType, scaleMin, scaleRand, smokeFPSMin, smokeFPSRand, smokeLayer, smokeShadowLayer, smokeWindScale, isFarAway);
            } catch(...) {}

            smokeWaitTime = (float(rand()) / float(RAND_MAX)) * waitRand + waitMin;

            smokeCurWait = 0.0f;
        }
    }

    TrajectoryParticle2D::sim();

} // end ChunkTrajectoryParticle2D::sim

// createGroundChunks
//---------------------------------------------------------------------------
void ChunkTrajectoryParticle2D::createGroundChunks()
{
    PackedSurface tempPackedSurface;
    Surface       tempSurface;

    // Build a table of the ground colors.
    tempSurface.create(2, 2, 256);
    for (unsigned int i = 0; i < tempSurface.getNumFrames(); i++) {
        tempSurface.setFrame(i);
        tempSurface.fill(i);

        if (i == 0) {
            tempSurface.fill(1);
        }
    }

    tempPackedSurface.pack(tempSurface);

    char chunkPathPAK[] = "pics/particles/chunks/pak/";
    char strBuf[256];
    sprintf(strBuf, "%sgroundChunks.pak", chunkPathPAK);
    tempPackedSurface.save(strBuf);
} // end ChunkTrajectoryParticle2D::createGroundChunks

// createPAKFiles
//---------------------------------------------------------------------------
void ChunkTrajectoryParticle2D::createPAKFiles()
{
#if 0
    createGroundChunks();
    createBurnGroundChunks();
    createUnitBodyGreenChunks();
    createUnitBodyGrayChunks();
#endif

    staticPackedGroundChunks.load("pics/particles/chunks/pak/groundChunks.pak");
    staticPackedBurnGroundChunks.load("pics/particles/chunks/pak/greenUnitChunks.pak");
    staticPackedUnitGreenChunks.load("pics/particles/chunks/pak/grayUnitChunks.pak");
    staticPackedUnitGrayChunks.load("pics/particles/chunks/pak/burnGroundChunks.pak");

} // end ChunkTrajectoryParticle2D::createPAKFiles


// createUnitBodyGrayChunks
//---------------------------------------------------------------------------
void ChunkTrajectoryParticle2D::createUnitBodyGrayChunks()
{
    // Blt the green titan body onto a surface to pull colors from.
    Surface tempUnitBodyGray;
    UnitProfileSprites * uprofile = UnitProfileSprites::getUnitProfileSprites(2);

    tempUnitBodyGray.create(uprofile->bodySprite.getWidth(), uprofile->bodySprite.getHeight(), 1);
    tempUnitBodyGray.fill(0);
    uprofile->bodySprite.blt(tempUnitBodyGray, 0, 0);

    // Build a table of the green unit colors.
    Surface tempSurface(2, 2, 64);

    unsigned int x = 0;
    unsigned int y = 0;

    for (unsigned int i = 0; i < tempSurface.getNumFrames(); i++) {
        tempSurface.setFrame(i);

        while (tempUnitBodyGray.getPixel(x, y) == 0) {
            x++;

            if (x > tempUnitBodyGray.getWidth() - 1) {
                x = 0;
                y++;
            } else if (y > tempUnitBodyGray.getHeight() - 1) {
                assert(false);
            }
        }

        tempSurface.fill(tempUnitBodyGray.getPixel(x, y));
    }

    PackedSurface tempPackedSurface;
    tempPackedSurface.pack(tempSurface);

    char chunkPathPAK[] = "pics/particles/chunks/pak/";
    char strBuf[256];
    sprintf(strBuf, "%sgrayUnitChunks.pak", chunkPathPAK);
    tempPackedSurface.save(strBuf);

} // end ChunkTrajectoryParticle2D::createUnitBodyGrayChunks

// createUnitBodyGreenChunks
//---------------------------------------------------------------------------
void ChunkTrajectoryParticle2D::createUnitBodyGreenChunks()
{

    // Blt the green titan body onto a surface to pull colors from.
    Surface tempUnitBodyGray;
    UnitProfileSprites * uprofile = UnitProfileSprites::getUnitProfileSprites(0);

    tempUnitBodyGray.create(uprofile->bodySprite.getWidth(), uprofile->bodySprite.getHeight(), 1);
    tempUnitBodyGray.fill(0);
    uprofile->bodySprite.blt(tempUnitBodyGray, 0, 0);

    // Build a table of the green unit colors.
    Surface tempSurface(2, 2, 64);

    unsigned int x = 0;
    unsigned int y = 0;

    for (unsigned int i = 0; i < tempSurface.getNumFrames(); i++) {
        tempSurface.setFrame(i);

        while (tempUnitBodyGray.getPixel(x, y) == 0) {
            x++;

            if (x > tempUnitBodyGray.getWidth() - 1) {
                x = 0;
                y++;
            } else if (y > tempUnitBodyGray.getHeight() - 1) {
                assert(false);
            }
        }

        tempSurface.fill(tempUnitBodyGray.getPixel(x, y));
    }

    PackedSurface tempPackedSurface;
    tempPackedSurface.pack(tempSurface);

    char chunkPathPAK[] = "pics/particles/chunks/pak/";
    char strBuf[256];
    sprintf(strBuf, "%sgreenUnitChunks.pak", chunkPathPAK);
    tempPackedSurface.save(strBuf);

} // end ChunkTrajectoryParticle2D::createUnitBodyGreenChunks

// createBurnGroundChunks
//---------------------------------------------------------------------------
void ChunkTrajectoryParticle2D::createBurnGroundChunks()
{
    Surface       tempSurface;
    tempSurface.create(2, 2, 26);

    int curFrame = 0;

    // Browns.
    tempSurface.fill(Palette::findNearestColor(204, 144, 9));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(212, 158, 29));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(149, 103, 19));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(170, 116, 17));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(173, 126, 44));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(206, 161, 75));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(235, 202, 150));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(190, 112, 0));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(229, 191, 136));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(140, 79, 1));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(156, 133, 101));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(213, 176, 135));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(107, 81, 57));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(141, 63, 0));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(149, 147, 146));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(22, 10, 7));
    tempSurface.setFrame(curFrame++);
    tempSurface.fill(Palette::findNearestColor(111, 108, 109));

    PackedSurface tempPackedSurface;
    tempPackedSurface.pack(tempSurface);

    char chunkPathPAK[] = "pics/particles/chunks/pak/";
    char strBuf[256];
    sprintf(strBuf, "%sburnGroundChunks.pak", chunkPathPAK);
    tempPackedSurface.save(strBuf);

} // end ChunkTrajectoryParticle2D::createBurnGroundChunks

// Greens.
//tempSurface.setFrame(curFrame++);
//tempSurface.fill(Palette::findNearestColor(SDL_Color(26, 88, 36)));
//tempSurface.setFrame(curFrame++);
//tempSurface.fill(Palette::findNearestColor(SDL_Color(58, 74, 60)));
//tempSurface.setFrame(curFrame++);
//tempSurface.fill(Palette::findNearestColor(SDL_Color(185, 235, 190)));
//tempSurface.setFrame(curFrame++);
//tempSurface.fill(Palette::findNearestColor(SDL_Color(42, 58, 43)));
//tempSurface.setFrame(curFrame++);
//tempSurface.fill(Palette::findNearestColor(SDL_Color(9, 149, 17)));
//tempSurface.setFrame(curFrame++);
//tempSurface.fill(Palette::findNearestColor(SDL_Color(31, 166, 36)));
//tempSurface.setFrame(curFrame++);
//tempSurface.fill(Palette::findNearestColor(SDL_Color(133, 180, 133)));
//tempSurface.setFrame(curFrame++);
//tempSurface.fill(Palette::findNearestColor(SDL_Color(141, 179, 136)));
//tempSurface.setFrame(curFrame++);
//tempSurface.fill(Palette::findNearestColor(SDL_Color(147, 159, 115)));
