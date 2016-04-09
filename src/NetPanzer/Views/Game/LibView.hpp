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
#ifndef __LibView_hpp__
#define __LibView_hpp__

#include "GameTemplateView.hpp"
#include "2D/Surface.hpp"
#include "Views/Components/CheckBox.hpp"

extern bool gDrawGameTiles;
extern bool gDrawUnitTips;

enum
{
    LIBVIEW_MODE_SURFACE_INFO,
    LIBVIEW_MODE_PARTICLE_INFO,
    LIBVIEW_MODE_ENVIRONMENT_INFO
};

//--------------------------------------------------------------------------
class LibView : public GameTemplateView
{
protected:
    void drawSurfaceInfo(Surface &clientArea, iXY pos);
    void drawParticleInfo(Surface &clientArea, iXY pos);
    void drawEnvironmentInfo(Surface &dest, iXY pos);

    CheckBox * checkBoxAllowParticleGeneration;
    CheckBox * checkBoxAllowTimeSlice;
    CheckBox * checkBoxAllowSpanBlitting;
    CheckBox * checkBoxParticlesCanHaveSmoke;
    CheckBox * checkBoxSolidColorExplosionParticles;
    CheckBox * checkBoxParticleInterfaceSim;
    CheckBox * checkBoxSolidBackground;
    CheckBox * checkBoxDrawExplosionParticleCount;

public:
    LibView();
    virtual ~LibView()
    {}

    virtual void doDraw(Surface &windowArea, Surface &clientArea);
    virtual void rMouseDrag(const iXY &downPos, const iXY &prevPos, const iXY &newPos);
    virtual void doActivate();
    virtual void actionPerformed(mMouseEvent me);

    static int displayMode;
}; // end LibView

#endif // end __LibView_hpp__
