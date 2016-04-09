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

#ifndef __SnowParticleSystem2D_hpp__
#define __SnowParticleSystem2D_hpp__



#include "ParticleSystem2D.hpp"
#include "SnowParticle2D.hpp"


class SnowParticleSystem2D : public ParticleSystem2D
{
    friend class cSnowParticle2D;

public:
    SnowParticleSystem2D(fXYZ pos);

protected:
    enum { SNOW_PARTICLE_COUNT = 100 };

    float           combinedTime;
    float           totalTime;
    SnowParticle2D *snowParticles;

    static int snowParticleCount;


    void init ();
    void reset();

    virtual void sim();
    virtual void draw(const Surface &dest, SpriteSorter &sorter);

}
; // end SnowParticleSystem2D

#endif // __SnowParticleSystem2D_hpp__
