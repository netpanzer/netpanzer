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

#ifndef TEST_LIB

#include "Particles/Particle2D.hpp"

#include "2D/PackedSurface.hpp"
#include "Interfaces/WorldViewInterface.hpp"
#include "Util/Exception.hpp"
#include "Util/TimerInterface.hpp"

// Static variables.
std::vector<Particle2D*> Particle2D::particles;
int Particle2D::frameCount = 0;
int Particle2D::peakCount = 0;
int Particle2D::bltTo = BLT_TO_SPRITE_SORTER;
int Particle2D::createParticles = 1;
int Particle2D::drawParticles = 1;

// Particle2D
//---------------------------------------------------------------------------
Particle2D::Particle2D(const fXYZ &pos) {
  reset();

  Particle2D::pos = pos;
  vectorIndex = static_cast<size_t>(-1);
  insertMe();
}  // end Particle2D

// ~Particle2D
//---------------------------------------------------------------------------
Particle2D::~Particle2D() { removeMe(); }  // end Particle2D::~Particle2D

// reset
//---------------------------------------------------------------------------
void Particle2D::reset() {
  age = 0.0f;
  lifetime = 0.0f;
  isAlive = true;
  direction.zero();
  pos.zero();
  velocity.zero();
  acceleration.zero();
  layer = 6;
  shadowLayer = 5;
  scale = 1.0f;
  index = 0;
  FPSMin = 4;
  FPSRand = 0;

}  // end Particle2D::reset

// insertMe
//---------------------------------------------------------------------------
// Purpose: Inserts a new particle into the vector.
//---------------------------------------------------------------------------
void Particle2D::insertMe() {
  // If we're inserting, we should not already be in the vector
  assert(vectorIndex == static_cast<size_t>(-1));

  // Add to vector
  vectorIndex = particles.size();
  particles.push_back(this);

  frameCount++;

  if (frameCount > peakCount) {
    peakCount = frameCount;
  }
}  // end Particle2D::insertMe

// removeMe
//---------------------------------------------------------------------------
// Purpose: Removes the particle from the vector using swap-and-pop.
//---------------------------------------------------------------------------
void Particle2D::removeMe() {
  // Check if we're actually in the vector
  if (vectorIndex == static_cast<size_t>(-1)) return;

  // Swap with last element and pop (O(1) removal)
  size_t lastIndex = particles.size() - 1;
  if (vectorIndex != lastIndex) {
    particles[vectorIndex] = particles[lastIndex];
    particles[vectorIndex]->vectorIndex = vectorIndex;
  }
  particles.pop_back();
  vectorIndex = static_cast<size_t>(-1);

  frameCount--;

}  // end Particle2D::removeMe

// removeAll
//---------------------------------------------------------------------------
void Particle2D::removeAll() {
  // ~Particle2D() calls removeMe(), which mutates 'particles' via swap-and-pop.
  // Iterating the vector while it shrinks reads past the end (AddressSanitizer
  // reports a container-overflow) and skips the particles that get swapped down
  // into slots the loop has already passed, leaking them. Always delete the
  // last element instead; removeMe() then only has to pop_back().
  while (!particles.empty()) {
    delete particles.back();
  }
}  // end Particle2D::removeAll

// simAll
//---------------------------------------------------------------------------
void Particle2D::simAll() {
  // Iterate backwards so that swap-and-pop during deletion doesn't affect unprocessed particles
  for (size_t i = particles.size(); i > 0; --i) {
    Particle2D* particle = particles[i - 1];
    particle->sim();
  }
}  // end Particle2D::simAll

// drawAll
//---------------------------------------------------------------------------
void Particle2D::drawAll(const Surface &clientArea, SpriteSorter &sorter) {
  // draw() can delete the particle (see SparkParticle2D::draw), so this has the
  // same constraint as simAll(): iterate backwards by index. The element that
  // swap-and-pop moves is always one this loop has already visited, and new
  // particles are appended past the current index.
  for (size_t i = particles.size(); i > 0; --i) {
    Particle2D* particle = particles[i - 1];
    particle->draw(clientArea, sorter);
  }
}  // end Particle2D::drawAll

// draw
//---------------------------------------------------------------------------
// Purpose: Draws a single particle, no simulation.
//---------------------------------------------------------------------------
void Particle2D::draw(const Surface &, SpriteSorter &) {}  // end draw

// Particle2D::sim
//---------------------------------------------------------------------------
// Purpose: Handles the default sim of a particle.  Make sure if you call this
//          like this "Particle2D::sim();", that it is at the end of the calling
//          function.  Otherwise, when the delete occurs, you may try to access
//          a variable through a null pointer.  Bad...
//---------------------------------------------------------------------------
void Particle2D::sim() {
  age += TimerInterface::getTimeSlice();

  // -1 = infinity
  if (lifetime != -1) {
    if (!isAlive) {
      delete this;
      return;
    }
  }

}  // end Particle2D::sim

// getFPS
//--------------------------------------------------------------------------
int Particle2D::getFPS(int FPSmin, int FPSrand) {
  // Get the random particle fps.
  int FPS = FPSmin;

  if (FPSrand > 0) {
    FPS = (rand() % FPSrand) + FPSmin;
  }

  return FPS;
}  // end Particle2D::getFPS

// getPakIndex
//--------------------------------------------------------------------------
int Particle2D::getPakIndex(float scale, int pakImageCount) {
  int destIndex = (int)(scale * float(pakImageCount));

  if (destIndex > pakImageCount - 1) {
    destIndex = pakImageCount - 1;
  }

  return destIndex;
}  // end Particle2D::getPakIndex

// getScale
//--------------------------------------------------------------------------
float Particle2D::getScale(float scaleMin, float scaleRand) {
  return (float(rand()) / float(RAND_MAX)) * scaleRand + scaleMin;
}  // end Particle2D::getScale

// getLifetime
//--------------------------------------------------------------------------
float Particle2D::getLifetime(float lifetimeMin, float lifetimeRand) {
  return (float(rand()) / float(RAND_MAX)) * lifetimeRand + lifetimeMin;
}  // end Particle2D::getLifetime

// getFarAway
//--------------------------------------------------------------------------
int Particle2D::getFarAway(const fXYZ &worldPos) {
  // Get the distance of the particle from the gameView.
  iRect gameViewRect;
  WorldViewInterface::getViewWindow(&gameViewRect);

  iXY gameViewCenter;

  gameViewCenter.x =
      ((gameViewRect.max.x - gameViewRect.min.x) >> 1) + gameViewRect.min.x;
  gameViewCenter.y =
      ((gameViewRect.max.y - gameViewRect.min.y) >> 1) + gameViewRect.min.y;

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
  if (distanceFromGameView.x > speedUpDistance ||
      distanceFromGameView.x < -speedUpDistance ||
      distanceFromGameView.y > speedUpDistance ||
      distanceFromGameView.y < -speedUpDistance) {
    return 1;
  }

  // The particle must be near the screen.
  return 0;
}  // end Particle2D::getFarAway

#else

#include "Particles/Particle2D.hpp"
#include "test.hpp"

// Deleting a particle removes it from the vector that removeAll() walks, so
// removeAll() has to tolerate the container changing underneath it. Getting
// this wrong reads past the end of the vector, which aborts under
// AddressSanitizer, and leaves particles behind. See issue #286.
void testRemoveAllDeletesEveryParticle(void) {
  const int count = 8;

  for (int i = 0; i < count; i++) {
    // Each particle adds itself to the static list, which owns it from here.
    Particle2D *particle = new Particle2D(fXYZ(float(i), 0.0f, 0.0f));
    (void)particle;
  }
  assert(Particle2D::getFrameCount() == count);

  Particle2D::removeAll();
  assert(Particle2D::getFrameCount() == 0);

  // The list has to stay usable afterwards.
  Particle2D *particle = new Particle2D(fXYZ(0.0f, 0.0f, 0.0f));
  (void)particle;
  assert(Particle2D::getFrameCount() == 1);

  Particle2D::removeAll();
  assert(Particle2D::getFrameCount() == 0);

  return;
}

// main() must be defined with the args in this format, otherwise we may get an
// "undefined reference to SDL_main"
int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  testRemoveAllDeletesEveryParticle();
  return 0;
}

#endif
