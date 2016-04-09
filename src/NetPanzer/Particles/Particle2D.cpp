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


#include "Util/Exception.hpp"
#include "Particles/Particle2D.hpp"
#include "Util/TimerInterface.hpp"
#include "2D/PackedSurface.hpp"
#include "Interfaces/WorldViewInterface.hpp"


static Particle2D theRealZParticle2D(fXYZ(0, 0, 0));
Particle2D *const Particle2D::zParticle2D = &theRealZParticle2D;

// Static variables.
int   Particle2D::frameCount          = 0;
int   Particle2D::peakCount           = 0;
int   Particle2D::bltTo               = BLT_TO_SPRITE_SORTER;
int   Particle2D::createParticles     = 1;
int   Particle2D::drawParticles       = 1;

// Pre-allocation variables.
const  size_t MAX_PARTICLE_CLASS_SIZE = 512;
const  int    MAX_PARTICLES           = 3000;
//static size_t biggestParticle         = 0;

static Particle2D *firstAvailableParticle;
static char particleArray[MAX_PARTICLES][MAX_PARTICLE_CLASS_SIZE];

// operator new
//---------------------------------------------------------------------------
void *Particle2D::operator new(size_t numBytes)
{
    // Make sure particle list is initted.
    static int once = 0;
    if (!once) {
        once = 1;

        // Insert all particles into available list.
        firstAvailableParticle = (Particle2D *)particleArray[0];

        for (int i = 0; i < MAX_PARTICLES - 1; i++) {
            ((Particle2D *)particleArray[i])->next = (Particle2D *)particleArray[i + 1];
        }

        ((Particle2D *)particleArray[MAX_PARTICLES - 1])->next = 0;
    }

    // Check global particle disable flag.
    if (!createParticles) {
        throw "particles disabled";
    }

    // Check for trying to create a particle that's too big.
    if (numBytes > MAX_PARTICLE_CLASS_SIZE) {
        throw Exception("ERROR: Tried to create a particle with class size %d bytes, but max particle class object size is %d bytes", (int)numBytes,
                int(MAX_PARTICLE_CLASS_SIZE));
    }

    // Check if all slots used.
    if (firstAvailableParticle == 0) {
        throw "all particle slots used";
    }

    // Remove particle from available list and return it.
    Particle2D *p = firstAvailableParticle;
    firstAvailableParticle = firstAvailableParticle->next;

    return p;
} // end Particle2D::operator new

// operator delete
//---------------------------------------------------------------------------
void Particle2D::operator delete(void *block)
{
    // !TEST! Make sure the particle we re freeing was created using our operator new.
#ifdef _DEBUG
    {
        int n = ((char *)block - &particleArray[0][0]) / (sizeof(particleArray[0]));
        assert(n >= 0);
        assert(n < MAX_PARTICLES);
        assert(particleArray[n] == block);
    }
#endif

    // Insert particle into available list.
    Particle2D *p = (Particle2D *)block;
    p->next = firstAvailableParticle;
    firstAvailableParticle = p;
} // end Particle2D::operator delete

// Particle2D
//---------------------------------------------------------------------------
Particle2D::Particle2D(const fXYZ &pos)
{
    reset();

    Particle2D::pos = pos;

    if (this == zParticle2D) {
        prev = next = zParticle2D;
    } else {
        prev = next = 0;
        insertMe();
    }
} // end Particle2D

// ~Particle2D
//---------------------------------------------------------------------------
Particle2D::~Particle2D()
{
    removeMe();

} // end Particle2D::~Particle2D

// reset
//---------------------------------------------------------------------------
void Particle2D::reset()
{
    age             = 0.0f;
    lifetime        = 0.0f;
    isAlive         = true;
    direction.zero();
    pos.zero();
    velocity.zero();
    acceleration.zero();
    layer           = 6;
    shadowLayer     = 5;
    scale           = 1.0f;
    index           = 0;
    FPSMin          = 4;
    FPSRand         = 0;

} // end Particle2D::reset

// insertMe
//---------------------------------------------------------------------------
// Purpose: Inserts a new particle into the list.
//---------------------------------------------------------------------------
void Particle2D::insertMe()
{
    // If we're inserting, we should not already be in the list.
    assert(prev == 0);
    assert(next == 0);

    // Insert me into the list
    prev              = zParticle2D;
    next              = zParticle2D->next;
    zParticle2D->next = this;
    next->prev        = this;

    frameCount++;

    if (frameCount > peakCount) {
        peakCount = frameCount;
    }
} // end Particle2D::insertMe

// removeMe
//---------------------------------------------------------------------------
// Purpose: Removes the particle from the list.
//---------------------------------------------------------------------------
void Particle2D::removeMe()
{
    // removeMe from the list
    if (prev != 0) prev->next = next;
    if (next != 0) next->prev = prev;

    prev = next = this;

    frameCount--;

} // end Particle2D::removeMe

// removeAll
//---------------------------------------------------------------------------
void Particle2D::removeAll()
{
    // Go through and remove all the particles.
    Particle2D *e = zParticle2D->next;
    Particle2D *nextPtr;

    while (e != zParticle2D) {
        nextPtr = e->next;
        delete e;
        e = nextPtr;
    }
} // end Particle2D::removeAll

// simAll
//---------------------------------------------------------------------------
void Particle2D::simAll()
{
    // Go through and simulate all the particles.
    Particle2D *e = zParticle2D->next;
    Particle2D *nextPtr;

    while (e != zParticle2D) {
        nextPtr = e->next;
        e->sim();
        e = nextPtr;
    }
} // end Particle2D::simAll

// drawAll
//---------------------------------------------------------------------------
void Particle2D::drawAll(const Surface &clientArea, SpriteSorter &sorter)
{
    // Go through and draw all the particles.
    Particle2D *e = zParticle2D->next;
    Particle2D *nextPtr;


    while (e != zParticle2D) {
        nextPtr = e->next;
        e->draw(clientArea, sorter);
        e = nextPtr;
    }
} // end Particle2D::drawAll

// draw
//---------------------------------------------------------------------------
// Purpose: Draws a single particle, no simulation.
//---------------------------------------------------------------------------
void Particle2D::draw(const Surface&, SpriteSorter&)
{} // end draw

// Particle2D::sim
//---------------------------------------------------------------------------
// Purpose: Handles the default sim of a particle.  Make sure if you call this
//          like this "Particle2D::sim();", that it is at the end of the calling
//          function.  Otherwise, when the delete occurs, you may try to access
//          a variable through a null pointer.  Bad...
//---------------------------------------------------------------------------
void Particle2D::sim()
{
    age += TimerInterface::getTimeSlice();

    // -1 = infinity
    if (lifetime != -1) {
        if (!isAlive) {
            delete this;
            return;
        }
    }

} // end Particle2D::sim

// getFPS
//--------------------------------------------------------------------------
int Particle2D::getFPS(int FPSmin, int FPSrand)
{
    // Get the random particle fps.
    int FPS = FPSmin;

    if (FPSrand > 0) {
        FPS = (rand() % FPSrand) + FPSmin;
    }

    return FPS;
} // end Particle2D::getFPS

// getPakIndex
//--------------------------------------------------------------------------
int Particle2D::getPakIndex(float scale, int pakImageCount)
{
    int destIndex = (int) (scale * float(pakImageCount));

    if (destIndex > pakImageCount - 1) {
        destIndex = pakImageCount - 1;
    }

    return destIndex;
} // end Particle2D::getPakIndex

// getScale
//--------------------------------------------------------------------------
float Particle2D::getScale(float scaleMin, float scaleRand)
{
    return (float(rand()) / float(RAND_MAX)) * scaleRand + scaleMin;
} // end Particle2D::getScale

// getLifetime
//--------------------------------------------------------------------------
float Particle2D::getLifetime(float lifetimeMin, float lifetimeRand)
{
    return (float(rand()) / float(RAND_MAX)) * lifetimeRand + lifetimeMin;
} // end Particle2D::getLifetime

// getFarAway
//--------------------------------------------------------------------------
int Particle2D::getFarAway(const fXYZ &worldPos)
{
    // Get the distance of the particle from the gameView.
    iRect gameViewRect;
    WorldViewInterface::getViewWindow(&gameViewRect);

    iXY gameViewCenter;

    gameViewCenter.x = ((gameViewRect.max.x - gameViewRect.min.x) >> 1) + gameViewRect.min.x;
    gameViewCenter.y = ((gameViewRect.max.y - gameViewRect.min.y) >> 1) + gameViewRect.min.y;

    iXY distanceFromGameView;

    distanceFromGameView.x = int(worldPos.x) - gameViewCenter.x;
    distanceFromGameView.y = int(worldPos.z) - gameViewCenter.y;

    int speedUpDistance;

    // XXX hacked around
    int SCREEN_XPIX = 1024;
    int SCREEN_YPIX = 768;

    if ((SCREEN_XPIX == 640) && (SCREEN_YPIX == 480)) {
        speedUpDistance = 480;
    } else if ((SCREEN_XPIX == 800) && (SCREEN_YPIX == 600)) {
        speedUpDistance = 600;
    } else if ((SCREEN_XPIX == 1024) && (SCREEN_YPIX == 768)) {
        speedUpDistance = 768;
    } else if ((SCREEN_XPIX == 1280) && (SCREEN_YPIX == 1024)) {
        speedUpDistance = 1024;
    } else {
        speedUpDistance = 1280;
    }

    // Check to see if the distance is so far from the screen that it
    // can be sped up.
    if (distanceFromGameView.x >  speedUpDistance ||
            distanceFromGameView.x < -speedUpDistance ||
            distanceFromGameView.y >  speedUpDistance ||
            distanceFromGameView.y < -speedUpDistance) {
        return 1;
    }

    // The particle must be near the screen.
    return 0;
} // end Particle2D::getFarAway

