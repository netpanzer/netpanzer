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


#include "RadarPingParticle2D.hpp"


Surface RadarPingParticle2D::radarPingSprite;


// RadarPingParticle2D
//---------------------------------------------------------------------------
RadarPingParticle2D::RadarPingParticle2D(fXYZ pos) : Particle2D(pos)
{
    assert(radarPingSprite.getDoesExist());

    FPS = rand() % 20 + 20;

    // XXX
#if 0
    radarPingSurface.setTo(radarPingSprite);
    radarPingSurface.setFPS(FPS);
#endif

}
 // end RadarPingParticle2D::RadarPingParticle2D

// init
//---------------------------------------------------------------------------
void RadarPingParticle2D::init()
{
    radarPingSprite.create(48, 46, 6);

    radarPingSprite.setOffset(iXY(-radarPingSprite.getCenter().x, -radarPingSprite.getHeight()));

} // end RadarPingParticle2D::init

// draw
//---------------------------------------------------------------------------
void RadarPingParticle2D::draw(const Surface &dest, SpriteSorter &sorter)
{
    (void) dest;
    (void) sorter;
    // XXX
#if 0
    assert(radarPingSurface.getDoesExist());

    if (!radarPingSurface.nextFrame()) {
        isAlive = false;
        return;
    }

    if (Particle2D::bltTo == BLT_TO_SPRITE_SORTER) {
        // I can't use nextFrame() below, because there are multiple
        // particles which share the same Surface.
        radarPingSurface.setAttrib(iXY(pos.x, pos.z), 3);
        sorter.addSprite(&radarPingSurface);
    } else if (Particle2D::bltTo == BLT_TO_SURFACE) {
        iRect r(pos.x, pos.z, pos.x + radarPingSurface.getWidth(), pos.z + radarPingSurface.getHeight());
        dest.bltScale(radarPingSurface, r);
        //radarPingSurface.bltTrans(dest, pos.x, pos.z);
    } else {
        assert(false);
    }
#endif

} // end RadarPingParticle2D::draw
