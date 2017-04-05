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


#include "Util/Exception.hpp"
#include "FlameParticle2D.hpp"

PackedSurfaceList FlameParticle2D::staticPackedExplosion0;
PackedSurfaceList FlameParticle2D::staticPackedExplosion1;

const int explosionFPS = 18;

// FlameParticle2D
//---------------------------------------------------------------------------
FlameParticle2D::FlameParticle2D(	const fXYZ  &pos,
                                  float        scaleMin,
                                  float        scaleRand,
                                  float        lifetime,
                                  int          layer) : Particle2D(pos)
{
    if (lifetime <= 0.0f) {
        isAlive = false;
    }

    FlameParticle2D::layer = layer;

    scale = getScale(scaleMin, scaleRand);

    // There are 2 explosion images to choose from.
    int picNum = rand() % 2;

    if (picNum == 0) {
        index = getPakIndex(scale, staticPackedExplosion0.size());
        packedSurface.setData(* (staticPackedExplosion0[index]) );
    } else if (picNum == 1) {
        index = getPakIndex(scale, staticPackedExplosion1.size());
        packedSurface.setData(* (staticPackedExplosion1[index]));
    } else {
        assert(false);
    }

    // Check for accelerated flames.
    packedSurface.setFPS(getFPS(explosionFPS, 0));
} // end FlameParticle2D::FlameParticle2D

// loadPakFiles
//---------------------------------------------------------------------------
void FlameParticle2D::loadPakFiles()
{
    char pathExplosion0[] = "pics/particles/explosion/explosion0/pak/";

    if (!loadAllPAKInDirectory(pathExplosion0, staticPackedExplosion0)) {
        throw Exception("ERROR: Unable to load any exposion images in %s", pathExplosion0);
    }

    char pathExplosion1[] = "pics/particles/explosion/explosion1/pak/";

    if (!loadAllPAKInDirectory(pathExplosion1, staticPackedExplosion1)) {
        throw Exception("ERROR: Unable to load any exposion images in %s", pathExplosion1);
    }
}

void FlameParticle2D::unloadPakFiles()
{
    char pathExplosion0[] = "pics/particles/explosion/explosion0/pak/";

    if (!unloadAllPAKInDirectory(pathExplosion0, staticPackedExplosion0)) {
        throw Exception("ERROR: Unable to load any exposion images in %s", pathExplosion0);
    }

    char pathExplosion1[] = "pics/particles/explosion/explosion1/pak/";

    if (!unloadAllPAKInDirectory(pathExplosion1, staticPackedExplosion1)) {
        throw Exception("ERROR: Unable to load any exposion images in %s", pathExplosion1);
    }
}
// init
//---------------------------------------------------------------------------
void FlameParticle2D::init()
{
    loadPakFiles();
} // end FlameParticle2D::init

void FlameParticle2D::uninit()
{
    unloadPakFiles();
} // end FlameParticle2D::init



// draw
//---------------------------------------------------------------------------
void FlameParticle2D::draw(const Surface&, SpriteSorter& sorter)
{
    if (!isAlive) {
        return;
    }

    if (!packedSurface.nextFrame()) {
        isAlive = false;
        return;
    }

    packedSurface.setAttrib(iXY((int)pos.x, (int)pos.z), layer);
    sorter.addSprite(&packedSurface);

} // end FlameParticle2D::draw

