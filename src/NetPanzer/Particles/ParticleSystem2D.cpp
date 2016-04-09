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


#include <assert.h>
#include "ParticleSystem2D.hpp"
#include "Util/TimerInterface.hpp"
#include "ParticleSystemGlobals.hpp"


// Set up the info for the PARTICLE_SYSTEMS in the linked list.
static ParticleSystem2D theRealZParticleSystem2D(fXYZ(0, 0, 0), 0);
ParticleSystem2D *const ParticleSystem2D::zParticleSystem2D = &theRealZParticleSystem2D;

int ParticleSystem2D::particleSystemCount     = 0;
int ParticleSystem2D::peakParticleSystemCount = 0;
int ParticleSystem2D::bltTo = BLT_TO_SURFACE;


//---------------------------------------------------------------------------
ParticleSystem2D::ParticleSystem2D(fXYZ pos, int isFarAway /* = 0 */)
{
    reset();
    ParticleSystem2D::pos = pos;
    ParticleSystem2D::isFarAway = isFarAway;

    if (this == zParticleSystem2D) {
        prev = next = zParticleSystem2D;
    }	else {
        prev = next = 0;
        insertMe();
    }

} // end ParticleSystem2D

// reset
//---------------------------------------------------------------------------
void ParticleSystem2D::reset()
{
    isAlive         = true;
    pos.zero();
    age             = 0;
    lifetime        = 0;
    isFarAway       = 0;

} // end reset

// reset
//---------------------------------------------------------------------------
void ParticleSystem2D::sim()
{
    age += TimerInterface::getTimeSlice();

    if (!isAlive) {
        delete this;
        return;
    }
}

// ParticleSystem2D
//---------------------------------------------------------------------------
ParticleSystem2D::~ParticleSystem2D()
{
    removeMe();

} // end ParticleSystem2D

// insertMe
//---------------------------------------------------------------------------
// Purpose: Inserts a new particle system into the list.
//---------------------------------------------------------------------------
void ParticleSystem2D::insertMe()
{
    // If we're inserting, we should not already be in the list...
    assert(prev == 0);
    assert(next == 0);

    // Insert me into the list
    prev                    = zParticleSystem2D;
    next                    = zParticleSystem2D->next;
    zParticleSystem2D->next = this;
    next->prev              = this;
    particleSystemCount++;

    if (particleSystemCount > peakParticleSystemCount) {
        peakParticleSystemCount = particleSystemCount;
    }

} // end insertMe

// removeMe
//---------------------------------------------------------------------------
// Purpose: Removes the particle system from the list.
//---------------------------------------------------------------------------
void ParticleSystem2D::removeMe()
{
    // removeMe from the list
    if (prev != 0) {
        prev->next = next;
    }

    if (next != 0) {
        next->prev = prev;
    }

    prev = next = this;

    particleSystemCount--;

} // end removeMe

// removeAll
//---------------------------------------------------------------------------
void ParticleSystem2D::removeAll()
{
    // Go through and simulate all the particle systems.
    ParticleSystem2D *e = zParticleSystem2D->next;
    ParticleSystem2D *nextPtr;

    while (e != zParticleSystem2D) {
        nextPtr = e->next;
        delete e;
        e = nextPtr;
    }

} // end ParticleSystem2D::removeAll

// simAll
//---------------------------------------------------------------------------
void ParticleSystem2D::simAll()
{
    // Go through and simulate all the particle systems.
    ParticleSystem2D *e = zParticleSystem2D->next;
    ParticleSystem2D *nextPtr;

    while (e != zParticleSystem2D) {
        nextPtr = e->next;
        e->sim();
        e = nextPtr;
    }

} // end simAll

// drawAll
//---------------------------------------------------------------------------
void ParticleSystem2D::drawAll(const Surface &clientArea, SpriteSorter &sorter)
{
    // Go through and draw all the particle systems.
    ParticleSystem2D *e = zParticleSystem2D->next;
    ParticleSystem2D *nextPtr;

    while (e != zParticleSystem2D) {
        nextPtr = e->next;
        e->draw(clientArea, sorter);
        e = nextPtr;
    }

} // end drawAll

//---------------------------------------------------------------------------
#ifdef _DEBUG
void ParticleSystem2D::verifyList()
{
    /*		// Verify head/tail node
    		assert(zParticleSystem2D != 0);
    		assert(zParticleSystem2D->next != 0);
    		assert(zParticleSystem2D->prev != 0);
     
    		// Go through links and verify nodes match up
    		ParticleSystem2D *e = zParticleSystem2D;
    		int numLeftToCount = particleSystemCount+1;
        do
    		{
    			assert(e != 0);
    			assert(e->next != 0);
    			assert(e->prev != 0);
     
    			assert(e->prev->next == e);
    			assert(e->next->prev == e);
     
    			e = e->next;
          assert(numLeftToCount > 0);
          --numLeftToCount;
    		} while (e != zParticleSystem2D);
    	  if (numLeftToCount != 0) {
          LOG(("numLeftToCount: %d", numLeftToCount));
        }
        assert(numLeftToCount == 0);
    */
}
#endif
