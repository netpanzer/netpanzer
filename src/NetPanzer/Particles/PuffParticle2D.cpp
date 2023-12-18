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


#include "PuffParticle2D.hpp"
#include "2D/PackedSurface.hpp"
#include "Util/TimerInterface.hpp"
#include "ParticleSystemGlobals.hpp"
#include "Util/Exception.hpp"
#include "2D/Palette.hpp"
#include "Interfaces/GameConfig.hpp"

// Static images.
PackedSurfaceList PuffParticle2D::staticPackedSmokeLightPuff;
PackedSurfaceList PuffParticle2D::staticPackedSmokeDarkPuff;
PackedSurfaceList PuffParticle2D::staticPackedDirtPuff;

// PuffParticle2D
//---------------------------------------------------------------------------
PuffParticle2D::PuffParticle2D(	const fXYZ &pos,
                                const fXYZ &shadowPos,
                                PUFF_TYPE   particleType,
                                float       minScale,
                                float       randScale,
                                int         minFPS,
                                int         randFPS,
                                int         layer,
                                int         shadowLayer,
                                float       windScale /* = 1.0f */,
                                int) : WindParticle2D(pos, windScale)
{
    PuffParticle2D::shadowPos = shadowPos;
    userDefinedShadowPos      = true;

    create(particleType, minScale, randScale, minFPS, randFPS, layer, shadowLayer);

} // end PuffParticle2D::PuffParticle2D

// create
//---------------------------------------------------------------------------
void PuffParticle2D::create(	PUFF_TYPE particleType,
                             float     scaleMin,
                             float     scaleRand,
                             int       FPSmin,
                             int       FPSrand,
                             int       layer,
                             int       shadowLayer,
                             int       isFarAway /* = 0 */)
{
    assert(scaleMin >= 0.0);
    assert(scaleRand >= 0.0);

    scale = getScale(scaleMin, scaleRand);

    PuffParticle2D::layer       = layer;
    PuffParticle2D::shadowLayer = shadowLayer;
    PuffParticle2D::isFarAway   = isFarAway;

    index = getPakIndex(scale, staticPackedSmokeLightPuff.size());

    if (particleType == LIGHT) {
        packedSurface.setData( *(staticPackedSmokeLightPuff[index]) );
        packedSurfaceShadow.setData( *(staticPackedSmokeLightPuff[index]) );
    } else if (particleType == DARK) {
        packedSurface.setData( *(staticPackedSmokeDarkPuff[index]) );
        packedSurfaceShadow.setData( *(staticPackedSmokeDarkPuff[index]) );
    } else if (particleType == DIRT) {
        packedSurface.setData( *(staticPackedDirtPuff[index]) );
        packedSurfaceShadow.setData( *(staticPackedDirtPuff[index]) );
    } else {
        throw Exception("ERROR: Unsupported particleType.");
    }

    packedSurfaceShadow.setDrawModeBlend(&Palette::colorTableDarkenALittle);

    if (GameConfig::video_blendsmoke) {
        int randColorTable = rand() % 4;

        if (randColorTable == 0) {
            packedSurface.setDrawModeBlend(&Palette::colorTable2080);
        } else if(randColorTable == 1) {
            packedSurface.setDrawModeBlend(&Palette::colorTable4060);
        } else if(randColorTable == 2) {
            packedSurface.setDrawModeBlend(&Palette::colorTable6040);
        } else if(randColorTable == 3) {
            packedSurface.setDrawModeBlend(&Palette::colorTable8020);

        } else {
            assert(false);
        }

    } else {
        packedSurface.setDrawModeSolid();
    }

    packedSurface.setFPS(getFPS(FPSmin, FPSrand));

    // If the particles are far away, speed them up.
    if (isFarAway) {
        assert(packedSurface.getFPS() > 0);

        packedSurface.setFPS(packedSurface.getFPS() * 2.0f);
    }

    // Set the shadow FPS to match the non-shadow particle.
    packedSurfaceShadow.setFPS(packedSurface.getFPS());

} // end PuffParticle2D::create

// draw
//---------------------------------------------------------------------------
// Purpose: Draws a single puff particle, no simulation.
//---------------------------------------------------------------------------
void PuffParticle2D::draw(const Surface&, SpriteSorter &sorter)
{
    if (!packedSurface.nextFrame()) {
        isAlive = false;
        return;
    }

    packedSurfaceShadow.nextFrame();

    packedSurface.setAttrib(iXY((int) pos.x, (int) pos.z), layer);
    sorter.addSprite(&packedSurface);

    if (GameConfig::video_shadows) {
        if (!userDefinedShadowPos) {
            shadowPos.x = pos.x - ((float(index) /
                        float(staticPackedSmokeLightPuff.size())) * packedSurfaceShadow.getCurFrame() * 10);
        }

        packedSurfaceShadow.setAttrib(iXY((int) shadowPos.x, (int) shadowPos.z), shadowLayer);
        sorter.addSprite(&packedSurfaceShadow);
    }

} // end PuffParticle2D::draw

// init
//---------------------------------------------------------------------------
void PuffParticle2D::init()
{
    //loadTILFiles();
    loadPAKFiles();

    // Uncomment the following to produce packed puff particles.
    //pakFiles();

} // end PuffParticle2D::init

//---------------------------------------------------------------------------
void PuffParticle2D::loadPAKFiles()
{
    char pathSmokeLight[] = "pics/particles/puff/smokeLight/pak/";

    if (!loadAllPAKInDirectory(pathSmokeLight, staticPackedSmokeLightPuff)) {
        throw Exception("ERROR: Unable to load any smoke puff particle images in %s", pathSmokeLight);
    }

    char pathSmokeDark[] = "pics/particles/puff/smokeDark/pak/";

    if (!loadAllPAKInDirectory(pathSmokeDark, staticPackedSmokeDarkPuff)) {
        throw Exception("ERROR: Unable to load any smoke puff particle images in %s", pathSmokeDark);
    }

    char pathDirt[] = "pics/particles/puff/dirt/pak/";

    if (!loadAllPAKInDirectory(pathDirt, staticPackedDirtPuff)) {
        throw Exception("ERROR: Unable to load any dirt puff particle images in %s", pathDirt);
    }
}

//---------------------------------------------------------------------------

void PuffParticle2D::unloadPAKFiles()
{
    char pathSmokeLight[] = "pics/particles/puff/smokeLight/pak/";

    if (!unloadAllPAKInDirectory(pathSmokeLight, staticPackedSmokeLightPuff)) {
        throw Exception("ERROR: Unable to load any smoke puff particle images in %s", pathSmokeLight);
    }

    char pathSmokeDark[] = "pics/particles/puff/smokeDark/pak/";

    if (!unloadAllPAKInDirectory(pathSmokeDark, staticPackedSmokeDarkPuff)) {
        throw Exception("ERROR: Unable to load any smoke puff particle images in %s", pathSmokeDark);
    }

    char pathDirt[] = "pics/particles/puff/dirt/pak/";

    if (!unloadAllPAKInDirectory(pathDirt, staticPackedDirtPuff)) {
        throw Exception("ERROR: Unable to load any dirt puff particle images in %s", pathDirt);
    }
}



//---------------------------------------------------------------------------
void PuffParticle2D::loadTILFiles()
{
}
