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

#ifndef __PuffParticle2D_hpp__
#define __PuffParticle2D_hpp__

#include <vector>

#include "ParticleSystemGlobals.hpp"
#include "WindParticle2D.hpp"

// PuffParticle2D
//--------------------------------------------------------------------------
class PuffParticle2D : public WindParticle2D {
 protected:
  // Lists of packed surfaces.
  static PackedSurfaceList staticPackedSmokeLightPuff;
  static PackedSurfaceList staticPackedSmokeDarkPuff;
  static PackedSurfaceList staticPackedDirtPuff;

  void create(PUFF_TYPE particleType, float scaleMin, float scaleRand,
              int FPSMin, int FPSRand, int layer, int shadowLayer,
              int isFarAway = 0);

  fXYZ shadowPos;
  bool userDefinedShadowPos;

 public:
  // A minSize of 1.0f would be the original size of the image.
  // WindScale is how much the wind effects this particle.  1.0f is full.
  PuffParticle2D(const fXYZ &arg_pos, const fXYZ &arg_shadowPos, PUFF_TYPE particleType,
                 float scaleMin, float scaleRand, int arg_FPSMin, int arg_FPSRand,
                 int arg_layer, int arg_shadowLayer, float arg_windScale = 1.0f,
                 int arg_isFarAway = 0);

  PuffParticle2D(const fXYZ &arg_pos, PUFF_TYPE particleType, float scaleMin,
                 float scaleRand, int arg_FPSMin, int arg_FPSRand, int arg_layer,
                 int arg_shadowLayer, float arg_windScale = 1.0f, int arg_isFarAway = 0)
      : WindParticle2D(arg_pos, arg_windScale) {
    userDefinedShadowPos = false;
    shadowPos = arg_pos;

    create(particleType, scaleMin, scaleRand, arg_FPSMin, arg_FPSRand, arg_layer,
           arg_shadowLayer, arg_isFarAway);
  }

  static void init();
  static void loadPAKFiles();
  static void unloadPAKFiles();
  static void loadTILFiles();
  virtual void draw(const Surface &dest, SpriteSorter &sorter);

};  // end PuffParticle2D

#endif  // __PuffParticle2D_hpp__
