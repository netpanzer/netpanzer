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


#ifndef __FireParticleSystem2D_hpp__
#define __FireParticleSystem2D_hpp__



#include "ParticleSystem2D.hpp"
#include "FireParticle2D.hpp"


class FireParticleSystem2D : public ParticleSystem2D
{
public:
    FireParticleSystem2D(fXYZ pos, int isFarAway);

    void init();

    static void initColors();
    static Uint8 fireColor[12];

    FireParticle2D fireParticles[100];

protected:
    float combinedTime;
    float totalTime;

    void reset();

    virtual void sim();
    virtual void draw(const Surface &dest, SpriteSorter &sorter);

}
; // end FireParticleSystem2D

#endif // __FireParticleSystem2D_hpp__
