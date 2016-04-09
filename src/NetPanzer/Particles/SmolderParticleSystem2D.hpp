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

#ifndef __SmolderParticleSystem2D_hpp__
#define __SmolderParticleSystem2D_hpp__



#include "ParticleSystem2D.hpp"
#include "PuffParticle2D.hpp"


class SmolderParticleSystem2D : public ParticleSystem2D
{
public:
    SmolderParticleSystem2D(	const fXYZ  &pos,
                             const iRect &emitBounds,
                             float        lifetime,
                             float        waitMin,
                             float        waitMax,
                             PUFF_TYPE    particleType);

protected:
    iRect bounds;     // The generation area of the particles.
    float waitMax;    // The maximum wait for a new particle spawn.
    float waitMin;    // The minimum wait for a new particle spawn.
    int   waitDiff;   // The difference in the wait max and min in integer (for rand()).
    float waitTime;   // The time we have been waiting for a new spawn.

    PUFF_TYPE particleType; // Which type of particle for the smolder?

    virtual void sim();
    //virtual void draw(const Surface &dest, SpriteSorter &sorter);

}
; // end SmolderParticleSystem2D

#endif // __SmolderParticleSystem2D_hpp__
