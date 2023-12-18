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




#include "Resources/ResourceManager.hpp"

#include <vector>
#include <algorithm>

#include "Util/Exception.hpp"
#include "RankView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Classes/ScreenSurface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Classes/Network/PlayerNetMessage.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Classes/WorldInputCmdProcessor.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Views/Components/Label.hpp"

#define HEADER_HEIGHT 24
#define ENTRY_HEIGHT 20

#define TABLE_BORDER 2
#define TABLE_BORDER_START (HEADER_HEIGHT+TABLE_BORDER)
#define TABLE_START (HEADER_HEIGHT + TABLE_BORDER)

#define ALLY_START 4
#define FLAG_START 26
// characters * character width
#define NAME_START (6*8)

#define TABLE_HEADER_PIX_LEN (52*8)

#define WINDOW_WIDTH (TABLE_HEADER_PIX_LEN + ((DEFAULT_BORDER_SIZE+TABLE_BORDER) * 2 ) + 14+2+28+14)

static const char * table_header =
        "      Name                 Frags Deaths Points Objs.";

static const char * stats_format = "%-20s%6i%7i%7i%6i";


// RankView
//---------------------------------------------------------------------------
RankView::RankView() : GameTemplateView()
{
    setSearchName("RankView");
    setTitle("Rankings");
    setSubTitle(" - TAB");

    setAllowResize(false);
    moveTo(GameConfig::interface_rankposition_x, GameConfig::interface_rankposition_y);
    resize(iXY(WINDOW_WIDTH, 200));
    checkArea(iXY(screen->getWidth(),screen->getHeight()));

    // Define the scrollBar fot this view.
    scrollBar = new ScrollBar(ScrollBar::VERTICAL, 0, 1, 0, 100);
    if (scrollBar == 0) {
        throw Exception("ERROR: Unable to allocate the scrollBar.");
    }

    allyImage.loadBMP("pics/default/ally.bmp");
    allyRequestImage.loadBMP("pics/default/allyRequest.bmp");
    allyOtherImage.loadBMP("pics/default/allyOther.bmp");
    noAllyImage.loadBMP("pics/default/noAlly.bmp");
    colorImage.loadBMP("pics/default/playerColor.bmp");
    muteImageW.loadBMP("pics/default/mute_W.bmp");
    muteImageR.loadBMP("pics/default/mute_R.bmp");
    botImage.loadBMP("pics/default/Bot.bmp");
    humanImage.loadBMP("pics/default/Human.bmp");
    unknownImage.loadBMP("pics/default/Unknown.bmp");

    selected_line = -1;

} // end RankView::RankView

// doDraw
//---------------------------------------------------------------------------
void RankView::doDraw(Surface &viewArea, Surface &clientArea)
{
    unsigned int flagHeight = ResourceManager::getFlag(0)->getHeight();
    unsigned int newheight = HEADER_HEIGHT
                             + (TABLE_BORDER * 2)
                             + (ENTRY_HEIGHT * PlayerInterface::countPlayers())
                             + Surface::getFontHeight()
                             + (DEFAULT_BORDER_SIZE * 2);

    if ( newheight != (unsigned int)getSizeY() )
    {
        resize(WINDOW_WIDTH, newheight);
        return; // this frame draws nothing
    }

    bltViewBackground(viewArea);

    clientArea.drawButtonBorder(
            iRect(0, TABLE_BORDER_START, clientArea.getWidth()-1, clientArea.getHeight()-1),
            Color::gray64, Color::white);

    clientArea.bltStringShadowed(0, 16, table_header, Color::red, Color::gray64);
    drawPlayerStats(clientArea, flagHeight);

    View::doDraw(viewArea, clientArea);
} // end doDraw

class StatesSortByPoints
    : public std::binary_function<const PlayerState*, const PlayerState*, bool>
{
public:
    bool operator() (const PlayerState* state1, const PlayerState* state2) {
        return state1->getTotal() > state2->getTotal();
    }
};

class StatesSortByObjectives
    : public std::binary_function<const PlayerState*, const PlayerState*, bool>
{
public:
    bool operator() (const PlayerState* state1, const PlayerState* state2) {
        int p1 = state1->getObjectivesHeld();
        int p2 = state2->getObjectivesHeld();
        return p1 > p2 || (p1 == p2 && state1->getTotal() > state2->getTotal() );
    }
};


// drawPlayerStats
//---------------------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------------------
void RankView::drawPlayerStats(Surface &dest, unsigned int flagHeight)
{
    char statBuf[256];

    states.clear();
    PlayerID i;
    for( i = 0; i < PlayerInterface::getMaxPlayers(); ++i)
    {
        PlayerState* state = PlayerInterface::getPlayer(i);
        if ( state->isActive() )
        {
            states.push_back(state);
        }
    }

    switch(GameConfig::game_gametype)
    {
        case _gametype_objective:
        case _gametype_objectiveANDfraglimit:
            std::sort(states.begin(), states.end(), StatesSortByObjectives());
            break;
        case _gametype_timelimit:
        case _gametype_fraglimit:
        case _gametype_fraglimitORtimelimit:
            std::sort(states.begin(), states.end(), StatesSortByPoints());
            break;
    }

    int cur_line_pos = TABLE_START + ((ENTRY_HEIGHT - Surface::getFontHeight())/2);
    int flag_pos = TABLE_START + (int(ENTRY_HEIGHT - flagHeight))/2;
    Surface * flag = 0;
    int cur_state = 0;
    for(std::vector<const PlayerState*>::iterator i = states.begin();
            i != states.end(); ++i) {
        const PlayerState* state = *i;

        snprintf(statBuf, sizeof(statBuf),
                stats_format, state->getName().substr(0,20).c_str(),
                state->getKills(), state->getLosses(), state->getTotal(),
                state->getObjectivesHeld());
        dest.bltStringShadowed(NAME_START, cur_line_pos, statBuf,
                               (cur_state == selected_line)?Color::yellow:Color::gray224,
                               Color::gray64);

        flag = ResourceManager::getFlag(state->getFlag());
        flag->blt( dest, FLAG_START, flag_pos );
        if ( state->getID() != PlayerInterface::getLocalPlayerIndex() )
        {
            // XXX ALLY
            bool meWithHim = PlayerInterface::isSingleAllied(PlayerInterface::getLocalPlayerIndex(), state->getID());
            bool himWithMe = PlayerInterface::isSingleAllied(state->getID(), PlayerInterface::getLocalPlayerIndex());
            if ( meWithHim && himWithMe )
            {
                allyImage.bltTrans(dest, ALLY_START, flag_pos );
            }
            else if ( meWithHim )
            {
                allyRequestImage.bltTrans(dest, ALLY_START, flag_pos );
            }
            else if ( himWithMe )
            {
                allyOtherImage.bltTrans(dest, ALLY_START, flag_pos );
            }
            else
            {
                noAllyImage.bltTrans(dest, ALLY_START, flag_pos );
            }
        }

        colorImage.bltTransColor(dest, TABLE_HEADER_PIX_LEN+2+8, flag_pos, state->getColor());

        if ( state->getID() != PlayerInterface::getLocalPlayerIndex() )
        {
        mstate = PlayerInterface::getPlayer( state->getID() );
        if (mstate->getMute() == true) {
        muteImageR.bltTrans(dest, TABLE_HEADER_PIX_LEN+2+14+14, flag_pos );
        } else {
        muteImageW.bltTrans(dest, TABLE_HEADER_PIX_LEN+2+14+14, flag_pos );
        }

        }

        tstate = PlayerInterface::getPlayer( state->getID() );
        if (tstate->getClientType() == 1) {
        humanImage.bltTrans(dest, TABLE_HEADER_PIX_LEN+2+14+14+18, flag_pos );
        } else if (tstate->getClientType() == 2) {
        botImage.bltTrans(dest, TABLE_HEADER_PIX_LEN+2+14+14+18, flag_pos );
        } else if (tstate->getClientType() == 0) {
        humanImage.bltTrans(dest, TABLE_HEADER_PIX_LEN+2+14+14+18, flag_pos );
        } else {
        unknownImage.bltTrans(dest, TABLE_HEADER_PIX_LEN+2+14+14+18, flag_pos );
        }


        cur_line_pos += ENTRY_HEIGHT;
        flag_pos += ENTRY_HEIGHT;
        ++cur_state;
    }

} // end RankView::drawPlayerStats

void RankView::notifyMoveTo()
{
    GameConfig::interface_rankposition_x = min.x;
    GameConfig::interface_rankposition_y = min.y;
}

void RankView::lMouseDown(const iXY& pos)
{
    GameTemplateView::lMouseDown(pos);


    if ( pos.x >= TABLE_HEADER_PIX_LEN+2+14+12 && pos.x <= TABLE_HEADER_PIX_LEN+2+14+12+10 && pos.y >= TABLE_START )
    {
        unsigned int yposm = pos.y - TABLE_START;
        unsigned int lineposm = yposm / ENTRY_HEIGHT;
        if ( lineposm < states.size() )
        {

            unsigned int destplayerm = states[lineposm]->getID();
            //bool mutedp = states[lineposm]->getMute();
            unsigned int localplayerm = PlayerInterface::getLocalPlayerIndex();
            if ( destplayerm != localplayerm )
            {

                mstate = PlayerInterface::getPlayer( destplayerm );
                bool mutedp = mstate->getMute();

                if ( mutedp == true )
                {
                    mstate->setMute(false);
                    ConsoleInterface::postMessage(Color::orange, false, 0,
                                              "You've just unmuted %s.",
                                              mstate->getName().c_str());
                }
                else
                {
                    mstate->setMute(true);
                    ConsoleInterface::postMessage(Color::orange, false, 0,
                                              "You've just muted %s.",
                                              mstate->getName().c_str());

                }




            }
        }
    }

    // XXX ALLY
    if ( pos.x >= 4 && pos.x <= 24 && pos.y >= TABLE_START )
    {
        unsigned int ypos = pos.y - TABLE_START;
        unsigned int linepos = ypos / ENTRY_HEIGHT;
        if ( linepos < states.size() )
        {
            unsigned int destplayer = states[linepos]->getID();
            unsigned int localplayer = PlayerInterface::getLocalPlayerIndex();
            if ( destplayer != localplayer )
            {
                PlayerAllianceRequest allie_request;

                if ( PlayerInterface::isSingleAllied(localplayer, destplayer) )
                {
                    allie_request.set( localplayer, destplayer, _player_break_alliance);
                }
                else
                {
                    allie_request.set( localplayer, destplayer, _player_make_alliance);
                }

                CLIENT->sendMessage( &allie_request, sizeof(PlayerAllianceRequest));
            }
        }
    }


}

void RankView::mouseMove(const iXY & prevPos, const iXY &newPos)
{
    GameTemplateView::mouseMove(prevPos, newPos);
    selected_line = -1;
    if ( newPos.y >= TABLE_START )
    {
        selected_line = (newPos.y-TABLE_START) / ENTRY_HEIGHT;
    }
}

void RankView::doActivate()
{
    selected_line = -1;
}

void RankView::doDeactivate()
{
    selected_line = -1;
}

void RankView::processEvents()
{
    COMMAND_PROCESSOR.process(false);
}
