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
#include "EndRoundView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Classes/ScreenSurface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Classes/Network/PlayerNetMessage.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Objectives/ObjectiveInterface.hpp"

#include "Views/Components/Label.hpp"
#include "Views/Components/ViewGlobals.hpp"

#define HEADER_HEIGHT 70
#define ENTRY_HEIGHT 20

#define TABLE_BORDER 2
#define TABLE_BORDER_START (HEADER_HEIGHT+TABLE_BORDER)
#define TABLE_START (HEADER_HEIGHT + TABLE_BORDER)

#define ALLY_START 4
#define FLAG_START 26
// characters * character width
#define NAME_START (6*8)

#define TABLE_HEADER_PIX_LEN (52*8)

#define WINDOW_WIDTH (TABLE_HEADER_PIX_LEN + ((DEFAULT_BORDER_SIZE+TABLE_BORDER) * 2 ) + 14+2)

static const char * table_header =
        "      Name                 Frags Deaths Points Objs.";

static const char * stats_format = "%-20s%6i%7i%7i%6i";

EndRoundView::EndRoundView() : SpecialButtonView()
{
    setSearchName("EndRoundView");
    setTitle("Round stats");
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);
    setBordered(false);

    int x = (screen->getWidth() / 2) - 250;
    int y = (screen->getHeight() / 2) - 250;
    moveTo(iXY(x, y));
    resize(iXY(500, 500));
    checkArea(iXY(screen->getWidth(),screen->getHeight()));

    allyImage.loadBMP("pics/default/ally.bmp");
    allyRequestImage.loadBMP("pics/default/allyRequest.bmp");
    allyOtherImage.loadBMP("pics/default/allyOther.bmp");
    noAllyImage.loadBMP("pics/default/noAlly.bmp");
    colorImage.loadBMP("pics/default/playerColor.bmp");

    selected_line = -1;
    RectWinner = getClientRect();
    RectWinner.min.x = RectWinner.min.y = 0;
    RectStates = RectWinner;
    RectWinner.max.y = HEADER_HEIGHT-10;
    RectStates.min.y = HEADER_HEIGHT;

} // end EndRoundView::EndRoundView

// doDraw
//---------------------------------------------------------------------------
void EndRoundView::doDraw(Surface &viewArea, Surface &clientArea)
{
    unsigned int flagHeight = ResourceManager::getFlag(0)->getHeight();
    clientArea.BltRoundRect(RectWinner, 14, Palette::darkGray256.getColorArray());
    clientArea.FillRoundRect(RectWinner, 14, Color::red);
    clientArea.RoundRect(RectWinner,14, Color::gray);

    clientArea.BltRoundRect(RectStates, 14, Palette::darkGray256.getColorArray());
    clientArea.RoundRect(RectStates, 14, Color::gray);

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

class StatesSortByGoals
    : public std::binary_function<const PlayerState*, const PlayerState*, bool>
{
public:
    bool operator() (const PlayerState* state1, const PlayerState* state2) {
        int p1 = state1->getObjectivesHeld();
        int p2 = state2->getObjectivesHeld();

        float ratio = (float) GameConfig::game_occupationpercentage / 100.0;
        bool b1 = ( ( state1->getTotal() >= GameConfig::game_fraglimit ) && ( state1->getObjectivesHeld() >= ratio ) );
        bool b2 = ( ( state2->getTotal() >= GameConfig::game_fraglimit ) && ( state2->getObjectivesHeld() >= ratio ) );

        return b1 > b2 || (b1 == b2 && p1 > p2) || (b1 = b2 && p1 == p2 && state1->getTotal() > state2->getTotal() );
    }
};
// drawPlayerStats
//---------------------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------------------
void EndRoundView::drawPlayerStats(Surface &dest, unsigned int flagHeight)
{
    char statBuf[256];

    states.clear();
    PlayerID i;
    for( i = 0; i < PlayerInterface::getMaxPlayers(); ++i)
    {
        PlayerState* state = PlayerInterface::getPlayer(i);
        if( state->isActive() )
        {
            states.push_back(state);
        }
    }

    switch(GameConfig::game_gametype)
    {
        case _gametype_objective:
            std::sort(states.begin(), states.end(), StatesSortByObjectives());
            break;
        case _gametype_timelimit:
        case _gametype_fraglimit:
            std::sort(states.begin(), states.end(), StatesSortByPoints());
            break;
        case _gametype_objectiveANDfraglimit:
            std::sort(states.begin(), states.end(), StatesSortByGoals());
            break;
        case _gametype_fraglimitORtimelimit:
            std::sort(states.begin(), states.end(), StatesSortByPoints());
            break;
    }

    const PlayerState* Winner = states.front();
    snprintf(statBuf, sizeof(statBuf),
            "Winner is %s", Winner->getName().substr(0,20).c_str());
    dest.bltStringCenteredInRect(RectWinner,statBuf, Color::white);


    int cur_line_pos = TABLE_START + ((ENTRY_HEIGHT - Surface::getFontHeight())/2);
    int flag_pos = TABLE_START + (int(ENTRY_HEIGHT - flagHeight))/2;
    Surface * flag = 0;
    int cur_state = 0;

    dest.bltString(0, cur_line_pos, table_header, Color::gray);
    cur_line_pos += ENTRY_HEIGHT;
    flag_pos += ENTRY_HEIGHT;
    ++cur_state;
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

        colorImage.bltTransColor(dest, TABLE_HEADER_PIX_LEN+5, flag_pos, state->getColor());

        cur_line_pos += ENTRY_HEIGHT;
        flag_pos += ENTRY_HEIGHT;
        ++cur_state;
    }

}

void EndRoundView::lMouseDown(const iXY& pos)
{
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

void EndRoundView::mouseMove(const iXY & prevPos, const iXY &newPos)
{
    selected_line = -1;
    if ( newPos.y >= TABLE_START )
    {
        selected_line = (newPos.y-TABLE_START) / ENTRY_HEIGHT;
    }
}

void EndRoundView::doActivate()
{
    selected_line = -1;
}

void EndRoundView::doDeactivate()
{
    selected_line = -1;
}

void EndRoundView::checkResolution(iXY oldResolution, iXY newResolution)
{
    int x = (screen->getWidth() / 2) - 250;
    int y = (screen->getHeight() / 2) - 250;
    moveTo(iXY(x, y));
}
