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
//---------------------------------------------------------------------------

#ifndef __ParticleSystem2D_hpp__
#define __ParticleSystem2D_hpp__


#if _MSC_VER > 1000
	#pragma once
#endif


#include "Types/iRect.hpp"
#include "Types/iXY.hpp"
#include "Types/fXYZ.hpp"

class Surface;
class SpriteSorter;

//--------------------------------------------------------------------------
class ParticleSystem2D
{
public:
    ParticleSystem2D(fXYZ pos, int isFarAway = 0);
    virtual ~ParticleSystem2D();

    static ParticleSystem2D *const zParticleSystem2D;
    static ParticleSystem2D *firstParticleSystem2D;
    ParticleSystem2D *next;
    ParticleSystem2D *prev;

    static  void setBltTo(int bltTo)
    {
        ParticleSystem2D::bltTo = bltTo;
    }

    static void  removeAll();
    static void  simAll();
    static void  drawAll(const Surface &dest, SpriteSorter &sorter);
    static int   getParticleSystemCount()
    {
        return particleSystemCount;
    }
    static int   getPeakParticleSystemCount()
    {
        return peakParticleSystemCount;
    }
    static float getVelocity()
    {
        return velocity;
    }

    // Move these back to protected...........
    static float velocity;

protected:

    static int bltTo;
    static int particleSystemCount;
    static int peakParticleSystemCount;

    bool  isAlive;
    float age;             // How old the system is.
    float lifetime;        // How long the system will live.
    fXYZ  pos;
    int   isFarAway;       // Is the particle near the camera.  Used to reduce stuff.

    void reset();

    virtual void sim();
    virtual void draw(const Surface&, SpriteSorter&)
    {}

    void insertMe();
    void removeMe();

#ifdef _DEBUG
    static void verifyList();
#else
    inline static void verifyList()
    {}
#endif

}
; // end ParticleSystem2D

#endif // __ParticleSystem2D_hpp__
