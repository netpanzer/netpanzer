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
#include "Particles/FlashParticle2D.hpp"
#include "2D/Palette.hpp"

PackedSurface FlashParticle2D::staticPackedFlash;

// FlashParticle2D
//---------------------------------------------------------------------------
FlashParticle2D::FlashParticle2D(	const fXYZ  &pos,
                                  float scaleMin,
                                  float scaleRand,
                                  float lifetime,
                                  int   layer,
                                  bool  singleFrame /* = false */) : Particle2D(pos)
{
    FlashParticle2D::scale       = getScale(scaleMin, scaleRand);
    FlashParticle2D::singleFrame = singleFrame;
    FlashParticle2D::layer       = layer;
    FlashParticle2D::lifetime    = lifetime;

    packedSurface.setData(staticPackedFlash);
    packedSurface.setDrawModeBlend(&Palette::colorTableBrighten);

} // end FlashParticle2D::FlashParticle2D

// init
//---------------------------------------------------------------------------
void FlashParticle2D::init()
{
    staticPackedFlash.load("pics/particles/lights/pak/flash2.pak");
} // end FlashParticle2D::init

void FlashParticle2D::uninit()
{
    staticPackedFlash.unload("pics/particles/lights/pak/flash2.pak");
} // end FlashParticle2D::init
// draw
//---------------------------------------------------------------------------
void FlashParticle2D::draw(const Surface&, SpriteSorter& sorter)
{
    if (!isAlive) {
        return;
    }

    // We just use the explosionFireSurface for life span, so the
    // explosion and light last the same length of time.
    float curScale = scale * (1.0f - (age / lifetime));

    if (curScale > 1.0f) {
        curScale = 1.0f;

    } else if (curScale < 0.0f) {
        curScale = 0.0f;
    }

    int frame = getPakIndex(curScale, packedSurface.getFrameCount());

    packedSurface.setFrame(frame);

    packedSurface.setAttrib(iXY((int)pos.x, (int)pos.z), layer);
    sorter.addSprite(&packedSurface);

    // Since we only want a single frame of the flash, kill it after it is done.
    if (singleFrame) {
        isAlive = false;
    }

} // end FlashParticle2D::draw

// sim
//---------------------------------------------------------------------------
void FlashParticle2D::sim()
{
    if (age > lifetime) {
        isAlive = false;
    }

    Particle2D::sim();

} // end FlashParticle2D::sim
