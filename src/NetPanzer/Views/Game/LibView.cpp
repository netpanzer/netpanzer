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


#include "LibView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Particles/ParticleSystem2D.hpp"
#include "Particles/Particle2D.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Views/MainMenu/Multi/HostOptionsView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Util/UtilInterface.hpp"
#include "Util/TimerInterface.hpp"
#include "2D/PackedSurface.hpp"
#include "Particles/ParticleInterface.hpp"
#include "2D/Span.hpp"
#include "GameView.hpp"
#include "Particles/CraterParticle2D.hpp"
#include "Classes/ScreenSurface.hpp"

bool gDrawGameTiles = true;
bool gDrawUnitTips  = false;


int LibView::displayMode = LIBVIEW_MODE_SURFACE_INFO;

static void bSurfaceInfo()
{
    LibView::displayMode = LIBVIEW_MODE_SURFACE_INFO;
}

static void bParticleInfo()
{
    LibView::displayMode = LIBVIEW_MODE_PARTICLE_INFO;
}


static void bEnvironmentInfo()
{
    LibView::displayMode = LIBVIEW_MODE_ENVIRONMENT_INFO;
}


// LibView
//---------------------------------------------------------------------------
LibView::LibView() : GameTemplateView()
{
    setSearchName("LibView");
    setTitle("Lib Stats");
    setSubTitle(" - F5");

    setAllowResize(false);
    setAllowMove(true);
    setDisplayStatusBar(false);
    setVisible(false);

    moveTo(0, 0);
    resize(325, 375);

    addButtonCenterText(iXY((getClientRect().getSize().x / 3) * 0, 0), getClientRect().getSize().x / 3, "Surface", "", bSurfaceInfo);
    addButtonCenterText(iXY((getClientRect().getSize().x / 3) * 1, 0), getClientRect().getSize().x / 3, "Particles", "", bParticleInfo);
    addButtonCenterText(iXY((getClientRect().getSize().x / 3) * 2, 0), getClientRect().getSize().x / 3, "Environment", "", bEnvironmentInfo);

    checkBoxAllowParticleGeneration = new CheckBox();
    checkBoxAllowParticleGeneration->setLabel("Allow Particle Generation");
    checkBoxAllowParticleGeneration->setLocation(0, 15);
    checkBoxAllowParticleGeneration->setState(Particle2D::getCreateParticles());
    add(checkBoxAllowParticleGeneration);

    checkBoxAllowTimeSlice = new CheckBox();
    checkBoxAllowTimeSlice->setLabel("Allow Time Slice");
    checkBoxAllowTimeSlice->setLocation(0, 30);
    checkBoxAllowTimeSlice->setState(gTimeSliceFlag);
    add(checkBoxAllowTimeSlice);

    checkBoxAllowSpanBlitting = new CheckBox();
    checkBoxAllowSpanBlitting->setLabel("Allow Blended Span Blitting");
    checkBoxAllowSpanBlitting->setLocation(0, 45);
    checkBoxAllowSpanBlitting->setState(allowSpanBlitting);
    add(checkBoxAllowSpanBlitting);

    checkBoxParticlesCanHaveSmoke = new CheckBox();
    checkBoxParticlesCanHaveSmoke->setLabel("Allow Explosion Particle Smoke");
    checkBoxParticlesCanHaveSmoke->setLocation(0, 60);
    checkBoxParticlesCanHaveSmoke->setState(ParticleInterface::gParticlesCanHaveSmoke);
    add(checkBoxParticlesCanHaveSmoke);

    checkBoxSolidColorExplosionParticles = new CheckBox();
    checkBoxSolidColorExplosionParticles->setLabel("Solid Color Explosion Particles");
    checkBoxSolidColorExplosionParticles->setLocation(0, 75);
    checkBoxSolidColorExplosionParticles->setState(ParticleInterface::gSolidColorExplosionParticles);
    add(checkBoxSolidColorExplosionParticles);

    checkBoxParticleInterfaceSim = new CheckBox();
    checkBoxParticleInterfaceSim->setLabel("Particle Interface Sim");
    checkBoxParticleInterfaceSim->setLocation(0, 90);
    checkBoxParticleInterfaceSim->setState(ParticleInterface::gTestSim);
    add(checkBoxParticleInterfaceSim);

    checkBoxSolidBackground = new CheckBox();
    checkBoxSolidBackground->setLabel("Solid Background");
    checkBoxSolidBackground->setLocation(0, 105);
    checkBoxSolidBackground->setState(GameView::gDrawSolidBackground);
    add(checkBoxSolidBackground);

    checkBoxDrawExplosionParticleCount = new CheckBox();
    checkBoxDrawExplosionParticleCount->setLabel("Draw Explosion Particle Count");
    checkBoxDrawExplosionParticleCount->setLocation(0, 120);
    checkBoxDrawExplosionParticleCount->setState(ParticleInterface::gDrawExplosionParticleCount);
    add(checkBoxDrawExplosionParticleCount);

} // end LibView::LibView

// doDraw
//---------------------------------------------------------------------------
void LibView::doDraw(Surface &viewArea, Surface &clientArea)
{
    //bltViewBackground(viewArea);

    int x       =   5;
    int y       = 135;
    //int yOffset =  15;
    bltViewBackground(viewArea);

    switch(displayMode) {
    case LIBVIEW_MODE_SURFACE_INFO: {
            drawSurfaceInfo(clientArea, iXY(x, y));
        }
        break;
    case LIBVIEW_MODE_PARTICLE_INFO: {
            drawParticleInfo(clientArea, iXY(x, y));
        }
        break;
    case LIBVIEW_MODE_ENVIRONMENT_INFO: {
            drawEnvironmentInfo(clientArea, iXY(x, y));
        }
        break;
    }

    View::doDraw(viewArea, clientArea);

} // end LibView::doDraw

//--------------------------------------------------------------------------
void LibView::drawSurfaceInfo(Surface &dest, iXY pos)
{
    int  yOffset = 15;

    char strBuf[256];

    dest.bltString(pos.x, pos.y, "-- Surface Info --", Color::green);
    pos.y += yOffset;

    sprintf(strBuf, "Alive Count: %d", Surface::getTotalSurfaceCount());
    dest.bltString(pos.x, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    sprintf(strBuf, "Mem: %d b, %d k, %3.2f MG", Surface::getTotalByteCount(), Surface::getTotalByteCount() / 1024, float(Surface::getTotalByteCount()) / 1024.0f / 1024.0f);
    dest.bltString(pos.x, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    dest.bltString(pos.x, pos.y, "-- Packed Surface Info --", Color::green);
    pos.y += yOffset;

    sprintf(strBuf, "Alive Count: %d", PackedSurface::getTotalSurfaceCount());
    dest.bltString(pos.x, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    sprintf(strBuf, "Draw Count:  %d", PackedSurface::totalDrawCount);
    dest.bltString(pos.x, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    sprintf(strBuf, "Mem: %d b, %d k, %3.2f MG", PackedSurface::getTotalByteCount(), PackedSurface::getTotalByteCount() / 1024, float(PackedSurface::getTotalByteCount()) / 1024.0f / 1024.0f);
    dest.bltString(pos.x, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    dest.bltString(pos.x, pos.y, "-- ColorTable Info --", Color::green);
    pos.y += yOffset;

    sprintf(strBuf, "Count:     %d", ColorTable::getTotalColorArrayCount());
    dest.bltString(pos.x, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    sprintf(strBuf, "Mem: %d b, %d k, %3.2f MG", ColorTable::getTotalByteCount(), ColorTable::getTotalByteCount() / 1024, float(ColorTable::getTotalByteCount()) / 1024.0f / 1024.0f);
    dest.bltString(pos.x, pos.y, strBuf, Color::white);
    pos.y += yOffset;
}

//--------------------------------------------------------------------------
void LibView::drawParticleInfo(Surface &dest, iXY pos)
{
    int  yOffset = 15;
    int  xOffset = 15;

    char strBuf[256];

    dest.bltString(pos.x, pos.y, "-- Particle Info --", Color::green);
    pos.y += yOffset;

    dest.bltString(pos.x, pos.y, "This frame count", Color::yellow);
    pos.y += yOffset;

    sprintf(strBuf, "Particle Systems: %d", ParticleSystem2D::getParticleSystemCount());
    dest.bltString(pos.x + xOffset, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    sprintf(strBuf, "Particles:        %d", Particle2D::getFrameCount());
    dest.bltString(pos.x + xOffset, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    dest.bltString(pos.x, pos.y, "Peak count", Color::yellow);
    pos.y += yOffset;

    sprintf(strBuf, "Particle Systems: %d", ParticleSystem2D::getPeakParticleSystemCount());
    dest.bltString(pos.x + xOffset, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    sprintf(strBuf, "Particles:        %d", Particle2D::getPeakCount());
    dest.bltString(pos.x + xOffset, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    dest.bltString(pos.x, pos.y, "Crater info", Color::yellow);
    pos.y += yOffset;

    float hitCount;
    float missCount;

    missCount = CraterParticle2D::getCacheMissCount();
    hitCount  = CraterParticle2D::getCacheHitCount();

    sprintf(strBuf, "Cache Miss:                %d (%%%2.2f)", int(missCount), 100.0f * (missCount / (hitCount + missCount)));
    dest.bltString(pos.x + xOffset, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    sprintf(strBuf, "Cache Hit:                 %d (%%%2.2f)", int(hitCount), 100.0f * (hitCount / (hitCount + missCount)));
    dest.bltString(pos.x + xOffset, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    missCount = ParticleInterface::getExplosionFlameFlashCullMissCount();
    hitCount  = ParticleInterface::getExplosionFlameFlashCullHitCount();

    dest.bltString(pos.x, pos.y, "Cull info", Color::yellow);
    pos.y += yOffset;

    sprintf(strBuf, "Explosion Flame Flash Miss:%d (%%%2.2f)", int(missCount), 100.0f * (missCount / (hitCount + missCount)));
    dest.bltString(pos.x + xOffset, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    sprintf(strBuf, "Explosion Flame Flash Hit: %d (%%%2.2f)", int(hitCount), 100.0f * (hitCount / (hitCount + missCount)));
    dest.bltString(pos.x + xOffset, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    missCount = ParticleInterface::getMuzzleSystemCullMissCount();
    hitCount  = ParticleInterface::getMuzzleSystemCullHitCount();

    sprintf(strBuf, "Muzzle System Miss:        %d (%%%2.2f)", int(missCount), 100.0f * (missCount / (hitCount + missCount)));
    dest.bltString(pos.x + xOffset, pos.y, strBuf, Color::white);
    pos.y += yOffset;

    sprintf(strBuf, "Muzzle System Hit:         %d (%%%2.2f)", int(hitCount), 100.0f * (hitCount / (hitCount + missCount)));
    dest.bltString(pos.x + xOffset, pos.y, strBuf, Color::white);
    pos.y += yOffset;
}

// rMouseDrag
//---------------------------------------------------------------------------
void LibView::rMouseDrag(const iXY&, const iXY &prevPos, const iXY &newPos)
{
    moveTo(min + newPos - prevPos);
    checkArea(iXY(screen->getWidth(),screen->getHeight()));
}

// doActivate
//---------------------------------------------------------------------------
void LibView::doActivate()
{
    Desktop::setActiveView(this);
} // end LibView::doActivate

// drawEnvironmentInfo
//---------------------------------------------------------------------------
void LibView::drawEnvironmentInfo(Surface &dest, iXY pos)
{
    int  yOffset = 15;
    //int  xOffset = 15;

    char strBuf[256];

    dest.bltString(pos.x, pos.y, "-- Environment Info --", Color::green);
    pos.y += yOffset;

    int windSpeed = GameConfig::game_windspeed;
    sprintf(strBuf, "Wind:   %s (%d pix/sec)",
            HostOptionsView::windSpeedString.c_str(), windSpeed);
    dest.bltString(pos.x, pos.y, strBuf, Color::yellow);
    pos.y += yOffset;

    int cloudCount = GameConfig::game_cloudcoverage;
    sprintf(strBuf, "Clouds: %s (%d allocated)",
            HostOptionsView::cloudCoverageString.c_str(), cloudCount);
    dest.bltString(pos.x, pos.y, strBuf, Color::yellow);
    pos.y += yOffset;

} // end LibView::drawEnvironmentInfo

// actionPerformed
//---------------------------------------------------------------------------
void LibView::actionPerformed(mMouseEvent me)
{
    if (me.getSource()==checkBoxAllowParticleGeneration) {
        Particle2D::setCreateParticles(checkBoxAllowParticleGeneration->getState());
    } else if (me.getSource()==checkBoxAllowTimeSlice) {
        gTimeSliceFlag = checkBoxAllowTimeSlice->getState();
    } else if (me.getSource()==checkBoxAllowSpanBlitting) {
        allowSpanBlitting = checkBoxAllowSpanBlitting->getState();
    } else if (me.getSource()==checkBoxParticlesCanHaveSmoke) {
        ParticleInterface::gParticlesCanHaveSmoke = checkBoxParticlesCanHaveSmoke->getState();
    } else if (me.getSource()==checkBoxSolidColorExplosionParticles) {
        ParticleInterface::gSolidColorExplosionParticles = checkBoxSolidColorExplosionParticles->getState();
    } else if (me.getSource()==checkBoxParticleInterfaceSim) {
        ParticleInterface::gTestSim = checkBoxParticleInterfaceSim->getState();
    } else if (me.getSource()==checkBoxSolidBackground) {
        GameView::gDrawSolidBackground = checkBoxSolidBackground->getState();
    } else if (me.getSource()==checkBoxDrawExplosionParticleCount) {
        ParticleInterface::gDrawExplosionParticleCount = checkBoxDrawExplosionParticleCount->getState();
    }

} // end LibView::actionPerformed
