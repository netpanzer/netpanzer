
#include "2D/Surface.hpp"


#include "GameView.hpp"

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


#include "GameView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Units/UnitInterface.hpp"
#include "Weapons/ProjectileInterface.hpp"
#include "Interfaces/MouseInterface.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Classes/WorldInputCmdProcessor.hpp"
#include "Interfaces/WorldViewInterface.hpp"
#include "Classes/ScreenSurface.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Particles/Particle2D.hpp"
#include "Particles/ParticleSystem2D.hpp"
#include "Views/Components/ViewGlobals.hpp"
#include "Particles/ParticleInterface.hpp"
#include "2D/PackedSurface.hpp"
#include "Views/Game/VehicleSelectionView.hpp"
#include "PowerUps/PowerUpInterface.hpp"

#include "Classes/ViewCamera.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/WorldViewInterface.hpp"

#include "Views/Components/InfoBar.hpp"
#include "Views/Components/MiniMap.hpp"

int GameView::gDrawSolidBackground = 0;

// GameView
//---------------------------------------------------------------------------
GameView::GameView() : View()
{
    setSearchName("GameView");
    setTitle("Game");
    setSubTitle("");

    setBordered(false);
    setAlwaysOnBottom(true);
    setAllowResize(false);
    setDisplayStatusBar(false);
    setVisible(false);

    resize(800,600);
    moveTo(iXY(0, 0));

    add(new InfoBar(0,0));
    // will add after input handling is done
    //add(new MiniMap(100,100));

} // end GameView::GameView

// setSize
//---------------------------------------------------------------------------
void GameView::setSize(iXY size)
{
    resize(size);
} // end GameView::setSize

// init
//---------------------------------------------------------------------------
void GameView::init()
{
    resize(iXY(screen->getWidth(), screen->getHeight()));
    moveTo(iXY(0, 0));
} // end GameView::init
void
GameView::checkResolution(iXY oldResolution, iXY newResolution)
{
    resize(iXY(newResolution.x, newResolution.y));
    moveTo(iXY(0,0));
}
// doDraw
//---------------------------------------------------------------------------
void GameView::doDraw(Surface &va, Surface &clientArea)
{
    // Added for debugging, accesible through LibView.
    //screen->fill(0);

    if (gDrawSolidBackground) {
        screen->fill(250);
    } else {
        drawMap(clientArea);
    }

    // Added for debugging, accesible through LibView.
    if (ParticleInterface::gTestSim) {
        ParticleInterface::testSim();
        //ParticleInterface::testSimText(viewArea);
    }

    iRect world_win;
    WorldViewInterface::getViewWindow( &world_win );
    SPRITE_SORTER.reset(world_win);

    PackedSurface::totalDrawCount = 0;
    ParticleSystem2D::drawAll(clientArea, SPRITE_SORTER );
    Particle2D::drawAll(clientArea, SPRITE_SORTER );

    UnitInterface::offloadGraphics( SPRITE_SORTER );
    ProjectileInterface::offloadGraphics( SPRITE_SORTER );
    PowerUpInterface::offloadGraphics( SPRITE_SORTER );

    SPRITE_SORTER.blitLists(&clientArea);

    VehicleSelectionView::drawMiniProductionStatus(clientArea);

    COMMAND_PROCESSOR.draw();

    // Make sure the console info is the last thing drawn.
    ConsoleInterface::update(clientArea);
    
    View::doDraw(va, clientArea);
} // end GameView::doDraw

// lMouseDown
//---------------------------------------------------------------------------
void GameView::doActivate()
{
    View::doActivate();
    MouseInterface::event_queue.clear();
    COMMAND_PROCESSOR.inFocus();
} // end GameView::doActivate

// processEvents
//---------------------------------------------------------------------------
void GameView::processEvents()
{
    COMMAND_PROCESSOR.process();
} // end GameView::processEvents

// mouseMove
//--------------------------------------------------------------------------
void GameView::mouseMove(const iXY & prevPos, const iXY &newPos)
{
    View::mouseMove(prevPos, newPos);

    if (!MouseInterface::getButtonMask() && Desktop::getFocus() != this) {
        Desktop::setFocusView(this);
        //Desktop::setActiveView(this);
        MouseInterface::setCursor("default.bmp");
    }

} // end GameView::mouseMove

void
blitTile(Surface &dest, unsigned short tile, int x, int y)
{
    PIX * tileptr = TileInterface::getTileSet()->getTile(tile);
    
    int lines = 32;
    int columns = 32;
    
    if ( y < 0 )
    {
        lines = 32 + y;
        tileptr += ((32-lines)*32);
        y = 0;
    }
    
    if ( x < 0 )
    {
        columns = 32 + x;
        tileptr += (32-columns); // advance the unseen pixels
        x = 0;
    }
    
    PIX * destptr = dest.getFrame0();
    destptr += (y * dest.getPitch()) + x;

    
    if ( y + 32 > (int)dest.getHeight() )
    {
        lines = (int)dest.getHeight() - y;
    }
    
    if ( x + 32 > (int)dest.getWidth())
    {
        columns = (int)dest.getWidth() - x;
    }
    
    PIX * endptr = destptr + (lines * dest.getPitch());
    
    for ( /* nothing */ ; destptr < endptr; destptr += dest.getPitch())
    {
        memcpy(destptr,tileptr,columns);
        tileptr +=32;
    }
    
}

void
GameView::drawMap(Surface &window)
{
    TileSet * ts = TileInterface::getTileSet();
    iXY world;
    iXY map;
    
    WorldViewInterface::getMainCamera()->getViewStart(window.getWidth(), window.getHeight(),
                              &world.x, &world.y);
    MapInterface::pointXYtoMapXY( world, map );
        
    unsigned short tile_size = ts->getTileXsize();
    
    long partial_y = world.y % tile_size;
    int y = 0;
    if ( partial_y )
    {
        y -= partial_y;
    }
    
    long partial_x = world.x % tile_size;
    int start_x = 0;
    if ( partial_x )
    {
        start_x -= partial_x;
    }
    
    unsigned int tile = 0;
    
    WorldMap * worldmap = MapInterface::getMap();
    
    unsigned short tmx;
    
    for ( ; y < (int)window.getHeight(); y += tile_size )
    {
        tmx = map.x;
        for ( int x = start_x; x < (int)window.getWidth(); x += tile_size )
        {
            tile = worldmap->getValue(tmx++, map.y);
            blitTile(window, tile, x, y);
        }
        map.y ++;
    }
}
