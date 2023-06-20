/*
Copyright (C) 2011 by Aaron Perez <aaronps@gmail.com>

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

#include "GFlagSelectionView.hpp"
#include "Classes/ScreenSurface.hpp"

#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/PlayerNetMessage.hpp"
#include "Classes/WorldInputCmdProcessor.hpp"

#include "Views/GameViewGlobals.hpp"
#include "Resources/ResourceManager.hpp"
#include "Views/Components/Button.hpp"
#include "Views/Components/Label.hpp"
#include "Views/Components/Desktop.hpp"

#include "2D/Palette.hpp"

#include <vector>
#include <string>

#define BORDER_SPACE 4

class GFlagButton : public Button
{
public:
    GFlagButton( int x, int y, Surface &s )
        : Button("gflag")
    {
        setLocation(x, y);
        setSize(s.getWidth(), s.getHeight());
        bimage.create(s.getWidth(), s.getHeight(), 1);
        s.blt(bimage, 0, 0);

        setExtraBorder();
        borders[1][0] = Color::red;
        borders[1][1] = Color::darkRed;
        borders[2][0] = Color::green;
        borders[2][1] = Color::darkGreen;
    }

    void actionPerformed( const mMouseEvent &e)
    {
        if ( e.getID() == mMouseEvent::MOUSE_EVENT_CLICKED )
        {
            bimage.frameToBuffer(GameConfig::player_flag_data,
                                 sizeof(GameConfig::player_flag_data));

            UpdatePlayerFlag upf;
            memcpy(upf.player_flag,
                   GameConfig::player_flag_data,
                   sizeof(GameConfig::player_flag_data));

            CLIENT->sendMessage(&upf, sizeof(upf));

            Desktop::setVisibility("GFlagSelectionView", false);
            COMMAND_PROCESSOR.Flagtimer.reset();
//            PlayerInterface::getLocalPlayer()->setStateActive();
        }
        else
        {
            Button::actionPerformed(e);
        }
    }
};

GFlagSelectionView::GFlagSelectionView() : View()
{
    setSearchName("GFlagSelectionView");
    setTitle("GFlag Selection");
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);
    setBordered(false);

    loaded = false;
}

void GFlagSelectionView::init()
{
    removeComponents();

    iRect viewrect = getClientRect();

    rect.min.x = (viewrect.getSizeX()/2) - 250;
    rect.min.y = (viewrect.getSizeY()/2) - 250;
    rect.max.x = rect.min.x + 500;
    rect.max.y = rect.min.y + 500;

    int tx = rect.min.x + 20;
    int ty = rect.min.y + 14 + (FLAG_HEIGHT - Surface::getFontHeight()) / 2;
    //add( new Label(tx, ty, "Current:", windowTextColor, windowTextColorShadow, true) );
    add( new Label(tx, ty, "Current:", Color::lightGray, Color::lightGray, false) );

    loc_player_flag.x = tx + Surface::getTextLength("Current:") + BORDER_SPACE;
    loc_player_flag.y = rect.min.y + 14;

    iXY flagStartOffset(rect.min.x + 14, rect.min.y + 14*3);

    int yOffset = FLAG_HEIGHT + 8;

    int x = flagStartOffset.x;
    int y = flagStartOffset.y;

    Surface game_flags;
    std::vector<std::string> flag_names;

    ResourceManager::loadAllFlags(game_flags, flag_names);

    unsigned int max_flags = game_flags.getNumFrames();
    for (unsigned int i = 0; i < max_flags; ++i)
    {
        game_flags.setFrame(i);
        add( new GFlagButton(x, y, game_flags) );

        x += FLAG_WIDTH + 8;

        if (x > flagStartOffset.x + rect.getSizeX() - 20 - FLAG_WIDTH)
        {
            x = flagStartOffset.x;
            y += yOffset;
        }
    }
    loaded = true;
}

void GFlagSelectionView::doDraw(Surface &viewArea, Surface &clientArea)
{
    clientArea.BltRoundRect(rect, 14, Palette::darkGray256.getColorArray());
    clientArea.RoundRect(rect,14, Color::gray);

    ResourceManager::getFlag(PlayerInterface::getLocalPlayerIndex())->blt(clientArea, loc_player_flag.x, loc_player_flag.y);

    View::doDraw(viewArea, clientArea);
} // end doDraw

void GFlagSelectionView::doActivate()
{
    if ( ! loaded )
    {
        init();
    }
    Desktop::setActiveView(this);
	}

void GFlagSelectionView::doDeactivate()
{
    if ( ! getVisible() )
    {
        removeComponents();
        loaded = false;
    }
}

void
GFlagSelectionView::checkResolution(iXY oldResolution, iXY newResolution)
{
    resize(iXY(newResolution.x, newResolution.y));
    moveTo(iXY(0,0));
}

void GFlagSelectionView::processEvents()
{
    COMMAND_PROCESSOR.process(false);
}
