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


#include "WindParticle2D.hpp"
#include "Physics.hpp"

//--------------------------------------------------------------------------
WindParticle2D::WindParticle2D(const fXYZ &pos, float windScale /* = 1.0f */)
        : Particle2D(pos)
{
    WindParticle2D::windScale = windScale;
} // end WindParticle2D::WindParticle2D

// WindParticle2D::sim
//--------------------------------------------------------------------------
void WindParticle2D::sim()
{
    pos += (Physics::wind.getDisplacement() * windScale);

    Particle2D::sim();
} // end WindParticle2D::sim
