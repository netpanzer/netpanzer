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
#ifndef __WeaponGlobals_hpp__
#define __WeaponGlobals_hpp__

#include "2D/PackedSurface.hpp"

extern int gShellVelocity;

extern int gMissleMaxVelocity;
extern int gMissleThrustForce;
extern int gBulletVelocity;

extern PackedSurface gShellPackedSurface;
extern PackedSurface gMissleSmallPackedSurface;
extern PackedSurface gMissleMediumPackedSurface;
extern PackedSurface gMissleThrustPackedSurface;
extern PackedSurface gMissleGroundLightPackedSurface;

extern int weaponLayer;
extern int weaponShadowLayer;
extern int weaponPuffLayer;

#endif  // __WeaponGlobals_hpp__
