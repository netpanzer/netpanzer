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


#include "DirtKickParticle2D.hpp"

Surface DirtKickParticle2D::dirtKickSprite;

const int dirtKickFPS = 24;

/** XXX distKickSurface doesn't seem to be defined ?!?! */


// DirtKickParticle2D
//---------------------------------------------------------------------------
DirtKickParticle2D::DirtKickParticle2D(fXYZ pos) : Particle2D(pos)
{
    assert(dirtKickSprite.getDoesExist());

    // XXX
    //dirtKickSurface.setTo(dirtKickSprite);

}
 // end DirtKickParticle2D::DirtKickParticle2D

// init
//---------------------------------------------------------------------------
void DirtKickParticle2D::init()
{
#if 0
    dirtKickSprite.create(iXY(48, 46), 48, 6);
    dirtKickSprite.extractPCX("pics/particles/dirtKick.pcx", 1, 0);

    dirtKickSprite.scale((int) (48 * 0.5f), (int) (46 * 0.5f));

    dirtKickSprite.setFPS(dirtKickFPS);
    dirtKickSprite.setOffset(iXY(-dirtKickSprite.getCenter().x, -dirtKickSprite.getPix().y));

#endif
} // end DirtKickParticle2D::init

// draw
//---------------------------------------------------------------------------
void DirtKickParticle2D::draw(const Surface&, SpriteSorter&)
{
    // XXX
    //assert(dirtKickSurface.getDoesExist());

    // XXX
#if 0
    if (!dirtKickSurface.nextFrame())
    {
        isAlive = false;
        return;
    }
#endif

    // XXX
#if 0
    // I can't use nextFrame() below, because there are multiple
    // particles which share the same Surface.
    dirtKickSurface.setAttrib(iXY(pos.x, pos.z), 3);
    sorter.addSprite(&dirtKickSurface);
#endif

} // end DirtKickParticle2D::draw
