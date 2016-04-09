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


#include "TemplateMuzzleSystem.hpp"
#include "VectorPuffParticle2D.hpp"
#include "Util/Math.hpp"
#include "Particles/FlashParticle2D.hpp"


// TemplateMuzzleSystem
//---------------------------------------------------------------------------
TemplateMuzzleSystem::TemplateMuzzleSystem(	const fXYZ         &pos,
        const fXYZ         &direction,
        const MuzzleSystem &m) : ParticleSystem2D(pos, isFarAway)
{

    //	enum MUZZLE_TYPE
    //	{
    //		SINGLE,
    //		TRIPLE,
    //	};
    //
    //	int    speedMin;
    //	int    speedRand;
    //	float  waitMin;
    //	float  waitRand;
    //	float  lifetimeMin;
    //	float  lifetimeRand;
    //	float  flashMinScale;
    //	float  flashRandScale;
    //	float  puffMinScale;
    //	float  puffRandScale;
    //	char  *name;
    //
    //	PUFF_TYPE   puffType;
    //	MUZZLE_TYPE muzzleType;
    //
    //	static float windScale;
    //	static int   layer;
    //	static int   shadowLayer;
    //	static int   flashLayer;

    // Chunk a flash in!
    try {
        new FlashParticle2D(pos, 0.3f, 0.2f, 0.1f, ExplosionSystem::flashLayer);
    } catch(...) {}

    if (m.muzzleType == MuzzleSystem::SINGLE) {
        try {
            new VectorPuffParticle2D(	pos,
                                      direction,
                                      m.puffMinScale,
                                      m.puffRandScale,
                                      m.speedMin,
                                      m.speedRand,
                                      m.waitMin,
                                      m.waitRand,
                                      m.lifetimeMin,
                                      m.lifetimeRand,
                                      MuzzleSystem::windScale);
        } catch(...) {}
    }
    else if (m.muzzleType == MuzzleSystem::TRIPLE) {
        try {
            new VectorPuffParticle2D(	pos,
                                      direction,
                                      m.puffMinScale,
                                      m.puffRandScale,
                                      m.speedMin,
                                      m.speedRand,
                                      m.waitMin,
                                      m.waitRand,
                                      m.lifetimeMin,
                                      m.lifetimeRand,
                                      MuzzleSystem::windScale);
        } catch(...) {}

        fXY eastDirection(Math::unitDirectionEast(fXY(direction.x, direction.z)));

        try {
            new VectorPuffParticle2D(	pos,
                                      fXYZ(eastDirection.x, 0, eastDirection.y),
                                      m.puffMinScale,
                                      m.puffRandScale,
                                      m.speedMin,
                                      m.speedRand,
                                      m.waitMin,
                                      m.waitRand,
                                      m.lifetimeMin,
                                      m.lifetimeRand,
                                      MuzzleSystem::windScale);
        } catch(...) {}

        fXY westDirection(Math::unitDirectionWest(fXY(direction.x, direction.z)));

        try {
            new VectorPuffParticle2D(	pos,
                                      fXYZ(westDirection.x, 0, westDirection.y),
                                      m.puffMinScale,
                                      m.puffRandScale,
                                      m.speedMin,
                                      m.speedRand,
                                      m.waitMin,
                                      m.waitRand,
                                      m.lifetimeMin,
                                      m.lifetimeRand,
                                      MuzzleSystem::windScale);
        } catch(...) {}
    }
    else {
        assert(false);
    }

    isAlive = false;

} // end TemplateMuzzleSystem::TemplateMuzzleSystem
