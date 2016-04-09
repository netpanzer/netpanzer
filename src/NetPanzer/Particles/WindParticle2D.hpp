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
#ifndef __WindParticle2D_hpp__
#define __WindParticle2D_hpp__

#include "Particles/Particle2D.hpp"
#include "Types/fXYZ.hpp"

// WindParticle2D
//--------------------------------------------------------------------------
class WindParticle2D : public Particle2D
{
protected:
    float windScale;  // How much does the wind effect me?

public:
    WindParticle2D(const fXYZ &pos, float windScale = 1.0f); // Used to increase or decrease wind. (Maybe depending on elevation.)

    virtual void sim();
}
; // end WindParticle2D

#endif // __WindParticle2D_hpp__
