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

#ifndef __FlashParticle2D_hpp__
#define __FlashParticle2D_hpp__


#include "Particles/Particle2D.hpp"
#include "2D/Surface.hpp"


// FlashParticle2D
//--------------------------------------------------------------------------
class FlashParticle2D : public Particle2D
{
public:
    FlashParticle2D(	const fXYZ  &pos,
                     float scaleMin,
                     float scaleRand,
                     float lifetime,
                     int   layer,
                     bool  singleFrame = false);

    static void init();
    static void uninit();

    static PackedSurface staticPackedFlash;

protected:
    // The size relative to the original image.  This is so we can keep
    // the images aspect ratio.
    bool  singleFrame;

    virtual void draw(const Surface &dest, SpriteSorter &sorter);
    virtual void sim();

}; // end FlashParticle2D

#endif // __FlashParticle2D_hpp__
