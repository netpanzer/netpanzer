/*
Copyright (C) 2003 Ivo Danihelka <ivo@danihelka.net>

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


#include <iostream>

#include "SDL.h"
#include <stdlib.h>
#include <assert.h>

#include "BotPlayer.hpp"

#include "Interfaces/PlayerInterface.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "Units/UnitInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Units/UnitBase.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Objectives/Objective.hpp"
#include "Util/Log.hpp"

//-----------------------------------------------------------------
BotPlayer::BotPlayer()
    : m_timer(1)
{
    /* empty */
}
//-----------------------------------------------------------------
    void
BotPlayer::processEvents()
{
    PlayerID playerIndex = isReady();
    if ( playerIndex != INVALID_PLAYER_ID )
    {
        UnitBase *unit = getRandomUnit(playerIndex);
        if (unit) {
            int unitTask = m_tasks.queryUnitTask(unit);
            if (unitTask != BotTaskList::TASK_MOVE) {
                unitOccupyOupost(unit);
            }

            // manual fire on closest enemy
            UnitBase *enemyUnit;
            if (UnitInterface::queryClosestEnemyUnit(&enemyUnit,
                        unit->unit_state.location, playerIndex))
            {
                manualFire(unit, enemyUnit->unit_state.location);
            }
        }
        else {
            LOGGER.debug("bot: empty unit list");
        }

        outpostProduce();
    }
}
//-----------------------------------------------------------------
/**
 * Whether is time to make action.
 * @return playerIndex or NONE_PLAYER
 */
PlayerID
BotPlayer::isReady()
{
    PlayerID playerIndex = INVALID_PLAYER_ID;
    if (m_timer.count()) {
        playerIndex = PlayerInterface::getLocalPlayerIndex();
        if ( playerIndex != INVALID_PLAYER_ID )
        {
            int unitCount = UnitInterface::getUnitCount(playerIndex);
            if (unitCount > 0) {
                m_timer.changePeriod(5.0 / unitCount);
            }
        }
    }

    return playerIndex;
}
//-----------------------------------------------------------------
/**
 * @return unit which belong to playerIndex
 */
UnitBase *
BotPlayer::getRandomUnit(PlayerID playerIndex)
{
    const std::vector<UnitBase*>& units 
        = UnitInterface::getPlayerUnits(playerIndex);

    if(units.size() == 0)
        return 0;
    
    size_t unitIndex = rand() % units.size();
    return units[unitIndex];
}

//-----------------------------------------------------------------
/**
 * @return list of active enemy players
 */
playerList_t
BotPlayer::getEnemyPlayers()
{
    playerList_t players;
    PlayerID localIndex = PlayerInterface::getLocalPlayerIndex();
    PlayerID max_players = PlayerInterface::getMaxPlayers();
    PlayerID player_index;
    for ( player_index = 0; player_index < max_players; ++player_index)
    {
        if (PlayerInterface::isPlayerActive(player_index)
                && localIndex != player_index
                && !PlayerInterface::isAllied(localIndex, player_index) // XXX ALLY
            )
        {
            players.push_back(player_index);
        }
    }
    return players;
}
//-----------------------------------------------------------------
/**
 * @return playerIndex or NONE_PLAYER
 */
PlayerID
BotPlayer::getRandomEnemyPlayer()
{
    PlayerID result = INVALID_PLAYER_ID;
    playerList_t players = getEnemyPlayers();
    if (players.size() > 0) {
        result = players[rand() % players.size()];
    }
    return result;
}
//-----------------------------------------------------------------
/**
 * @return enemy unit
 */
UnitBase *
BotPlayer::getRandomEnemy()
{
    UnitBase *enemyUnit = 0;
    PlayerID enemyPlayer = getRandomEnemyPlayer();
    if (enemyPlayer != INVALID_PLAYER_ID)
    {
        enemyUnit = getRandomUnit(enemyPlayer);
    }
    return enemyUnit;
}
//-----------------------------------------------------------------

static Objective* getRandomFreeOutpost()
{
    ObjectiveID num_objectives = ObjectiveInterface::getObjectiveCount();
    outpostList_t outposts;
    for ( int i = 0; i < num_objectives; ++i )
    {
        if ( ! ObjectiveInterface::getObjective(i)->occupying_player )
        {
            outposts.push_back(i);
        }
    }

    if ( ! outposts.empty() )
    {
        return ObjectiveInterface::getObjective(outposts[rand()%outposts.size()]);
    }
    return 0;
}

static Objective* getRandomEnemyOrFreeOutpost()
{
    ObjectiveID num_objectives = ObjectiveInterface::getObjectiveCount();
    outpostList_t outposts;
    for ( int i = 0; i < num_objectives; ++i )
    {
        if ( ObjectiveInterface::getObjective(i)->occupying_player == PlayerInterface::getLocalPlayer() )
        {
            outposts.push_back(i);
        }
    }

    if ( ! outposts.empty() )
    {
        return ObjectiveInterface::getObjective(outposts[rand()%outposts.size()]);
    }
    return 0;
}

//-----------------------------------------------------------------
/**
 * Occupy free or enemy oupost.
 */
void
BotPlayer::unitOccupyOupost(UnitBase *unit)
{
    Objective *outpost = getRandomFreeOutpost();
    if (!outpost)
    {
        outpost = getRandomEnemyOrFreeOutpost();
    }

    if (outpost)
    {
        // XXX hack, const occupation_pad_offset
        static const iXY occupation_pad_offset(224, 48);

        iXY map_loc;
        MapInterface::pointXYtoMapXY(
                outpost->location + occupation_pad_offset,
                map_loc);
        moveUnit(unit, map_loc);
    }
}
//-----------------------------------------------------------------
/**
 * Produce in factories
 */
void
BotPlayer::outpostProduce()
{
    ObjectiveID num_objectives = ObjectiveInterface::getObjectiveCount();
    Objective* obj;
    for ( int i = 0; i < num_objectives; ++i )
    {
        obj = ObjectiveInterface::getObjective(i);
        if ( obj->occupying_player
             && obj->occupying_player == PlayerInterface::getLocalPlayer()
             && obj->unit_generation_on_flag == false )
        {
            produceUnit( i, rand() % UnitProfileInterface::getNumUnitTypes() );
        }
    }
}
