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

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <assert.h>

#include "BotPlayer.hpp"

#include "BotTaskList.hpp"

#include "Interfaces/PlayerInterface.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "Units/UnitInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Units/UnitBase.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Objectives/Objective.hpp"
#include "Util/Log.hpp"


bool BotPlayer::attack_flag = false;
Objective * BotPlayer::target_outpost;
unsigned short BotPlayer::bot_objs = 0;
unsigned char BotPlayer::bot_status = _bot_idle;
bool BotPlayer::idle_units = true;
unsigned char BotPlayer::state_change = 0;
unsigned char BotPlayer::threat_level = 0;

//unsigned long BotPlayer::increment = 0;
bool BotPlayer::inrange = false;
bool BotPlayer::touch_flag = false;

//-----------------------------------------------------------------
BotPlayer::BotPlayer()
    : m_timer(0.2)
{
    //actionTimer.setTimeOut(1500);
    inactivityTimer.setTimeOut(5000);

    //actionTimer2.setTimeOut(1000);

    //pathTimer.setTimeOut(5000);
    //movingTimer.setTimeOut(800);
    //shootingTimer.setTimeOut(100);
    touchTimer.setTimeOut(5000);


    unsigned char action_speed = GameConfig::bot_action_speed;

    if (action_speed < 1 || action_speed > 7) {
        action_speed = 3;
        GameConfig::bot_action_speed = action_speed;
    }


    if (GameConfig::bot_class < 1 || GameConfig::bot_class > 2) {
        GameConfig::bot_class = 1;
    }

    //if (GameConfig::bot_class < 4) {

    switch ( action_speed )
    {

        case 1:
            actionTimer.setTimeOut(1800);
            break;
        case 2:
            actionTimer.setTimeOut(1500);
            break;
        case 3:
            actionTimer.setTimeOut(1300);
            break;
        case 4:
            actionTimer.setTimeOut(1100);
            break;
        case 5:
            actionTimer.setTimeOut(900);
            break;
        case 6:
            actionTimer.setTimeOut(750);
            break;
        case 7:
            actionTimer.setTimeOut(600);
            break;


    }

    //}



    /* empty */
}


//-----------------------------------------------------------------


//-----------------------------------------------------------------
    void
BotPlayer::processEvents()  // aka the 'Zombie' bot
{

    PlayerID playerIndex = isReady();
    if ( playerIndex != INVALID_PLAYER_ID )
    {


            if ( actionTimer.isTimeOut() ) {

            const std::vector<UnitBase*>& uc = UnitInterface::getPlayerUnits(playerIndex);
            if (uc.size() > 0) {
            UnitBase *unitsl = uc[rand() % uc.size()];



                for ( Uint8 i = 0; i < uc.size(); ++i)
                {
                UnitBase *unit = uc[i];


            if (GameConfig::game_base_limit == 0 || (GameConfig::game_base_limit > 0 && unit->player->getObjectivesHeld() <= GameConfig::game_base_limit)) {

            int unitTask = m_tasks.queryUnitTask(unitsl);
            if (unitTask != BotTaskList::TASK_MOVE) {
                //
                unitOccupyOupostNA(unitsl);
                //
            }

            UnitBase *enemyUnit;
            if (UnitInterface::queryClosestEnemyUnitInRange(&enemyUnit,
                        unit->unit_state.location, unit->unit_state.weapon_range, playerIndex))
            {

                signed char pos_x;
                signed char pos_y;


                float e_speed = enemyUnit->unit_state.speed_factor*enemyUnit->unit_state.speed_rate;

                float e_speed_d = e_speed*1.38;

                iXY delta  = enemyUnit->unit_state.location - unit->unit_state.location;
                long dist_v = long(delta.mag2());
                float dist_f = (sqrt(dist_v)/32)*5;


                orientationToOffset( enemyUnit->unit_state.orientation, &pos_x, &pos_y );

                unsigned long xs;
                unsigned long ys;

                unsigned long plus_f;

                if (enemyUnit->unit_state.moving) {

                if (enemyUnit->unit_state.orientation == 1 || enemyUnit->unit_state.orientation == 3 || enemyUnit->unit_state.orientation == 5 || enemyUnit->unit_state.orientation == 7) {
                plus_f = (int)((e_speed_d + dist_f)/3.2);
                } else {
                plus_f = (int)((e_speed + dist_f)/3.2);
                }


                xs = enemyUnit->unit_state.location.x+(pos_x*plus_f);
                ys = enemyUnit->unit_state.location.y+(pos_y*plus_f);
                } else {
                xs = enemyUnit->unit_state.location.x;
                ys = enemyUnit->unit_state.location.y;
                }


                iXY coords;
                coords.x = xs;
                coords.y = ys;
                //

                manualFire(unit, coords);

                //


            }

            } else {

/*
            if ( GameConfig::game_base_limit > 1 && unit->player->getObjectivesHeld() >= GameConfig::game_base_limit ) {
            Objective *chosen_outpost = getRandomOwned();
            if (chosen_outpost) {
                ObjectiveInterface::sendDisownObj(chosen_outpost->id, 0, PlayerInterface::getLocalPlayerIndex());
            }
            }
*/
            int unitTask = m_tasks.queryUnitTask(unitsl);
            UnitBase *target_enemyUnit;
            if (unitTask != BotTaskList::TASK_MOVE) {
                if (UnitInterface::queryClosestEnemyUnit(&target_enemyUnit,
                unitsl->unit_state.location, playerIndex))
            {
                iXY te_loc = target_enemyUnit->unit_state.location;
                iXY map_loc;
                MapInterface::pointXYtoMapXY( te_loc, map_loc);
                moveUnit(unitsl, map_loc);
            }

            }

/*
            int unitTask = m_tasks.queryUnitTask(unitsl);
            if (unitTask != BotTaskList::TASK_MOVE) {
                //
                unitOccupyEnemyOupostNA(unitsl);
                //
            }
*/
            UnitBase *enemyUnit;
            if (UnitInterface::queryClosestEnemyUnitInRange(&enemyUnit,
                        unit->unit_state.location, unit->unit_state.weapon_range, playerIndex))
            {

                signed char pos_x;
                signed char pos_y;


                float e_speed = enemyUnit->unit_state.speed_factor*enemyUnit->unit_state.speed_rate;

                float e_speed_d = e_speed*1.38;

                iXY delta  = enemyUnit->unit_state.location - unit->unit_state.location;
                long dist_v = long(delta.mag2());
                float dist_f = (sqrt(dist_v)/32)*5;


                orientationToOffset( enemyUnit->unit_state.orientation, &pos_x, &pos_y );

                unsigned long xs;
                unsigned long ys;

                unsigned long plus_f;

                if (enemyUnit->unit_state.moving) {

                if (enemyUnit->unit_state.orientation == 1 || enemyUnit->unit_state.orientation == 3 || enemyUnit->unit_state.orientation == 5 || enemyUnit->unit_state.orientation == 7) {
                plus_f = (int)((e_speed_d + dist_f)/3.2);
                } else {
                plus_f = (int)((e_speed + dist_f)/3.2);
                }


                xs = enemyUnit->unit_state.location.x+(pos_x*plus_f);
                ys = enemyUnit->unit_state.location.y+(pos_y*plus_f);
                } else {
                xs = enemyUnit->unit_state.location.x;
                ys = enemyUnit->unit_state.location.y;
                }


                iXY coords;
                coords.x = xs;
                coords.y = ys;


                manualFire(unit, coords);
            }

            }


            //} // inactivity

            }

        actionTimer.reset();

        outpostProduce();
        }

        } // timeout
    }
}

//-----------------------------------------------------------------

    void
BotPlayer::processEvents2() // aka the 'Fighter' Bot
{



   PlayerID playerIndex = isReady();
    if ( playerIndex != INVALID_PLAYER_ID )
    {

        unsigned char rand_dir = rand() % 4;
        unsigned short mov_offset = 320;
        unsigned short rand_margin = ((rand() %10) +1)*20;


        UnitBase *unitc = getRandomUnit(playerIndex);
        if (unitc) {

            if ( !inactivityTimer.isTimeOut() ) {
            if ( actionTimer.isTimeOut() ) {

            iXY plotonc;
            plotonc.x = unitc->unit_state.location.x;
            plotonc.y = unitc->unit_state.location.y;

            if (rand_dir == 0) {
                plotonc.x = plotonc.x + (mov_offset+rand_margin);
            } else if (rand_dir == 1) {
                plotonc.y = plotonc.y + (mov_offset+rand_margin);
            } else if (rand_dir == 2) {
                plotonc.x = plotonc.x - (mov_offset+rand_margin);
            } else {
                plotonc.y = plotonc.y - (mov_offset+rand_margin);
            }



            doGoThere(plotonc);
            actionTimer.reset();
            touch_flag = false;

            } // actionTimer

            } // inactivityTimer






           const std::vector<UnitBase*>& uc = UnitInterface::getPlayerUnits(playerIndex);
           unsigned char threat_count = 0;


           for ( Uint8 i = 0; i < uc.size(); ++i)
           {





            UnitBase *enemyUnit;
            if (UnitInterface::queryClosestEnemyUnitInRange(&enemyUnit,
                        uc[i]->unit_state.location, uc[i]->unit_state.weapon_range, playerIndex))
            {
                signed char pos_x;
                signed char pos_y;


                float e_speed = enemyUnit->unit_state.speed_factor*enemyUnit->unit_state.speed_rate;

                float e_speed_d = e_speed*1.38;

                iXY delta  = enemyUnit->unit_state.location - unitc->unit_state.location;
                long dist_v = long(delta.mag2());
                float dist_f = (sqrt(dist_v)/32)*3;


                orientationToOffset( enemyUnit->unit_state.orientation, &pos_x, &pos_y );

                unsigned long xs;
                unsigned long ys;

                unsigned long plus_f;

                if (enemyUnit->unit_state.moving) {

                if (enemyUnit->unit_state.orientation == 1 || enemyUnit->unit_state.orientation == 3 || enemyUnit->unit_state.orientation == 5 || enemyUnit->unit_state.orientation == 7) {
                plus_f = (int)((e_speed_d + dist_f)/3.0);
                } else {
                plus_f = (int)((e_speed + dist_f)/3.0);
                }


                xs = enemyUnit->unit_state.location.x+(pos_x*plus_f);
                ys = enemyUnit->unit_state.location.y+(pos_y*plus_f);
                } else {
                xs = enemyUnit->unit_state.location.x;
                ys = enemyUnit->unit_state.location.y;
                }


                iXY coords;
                coords.x = xs;
                coords.y = ys;




                if (uc[i]->unit_state.threat_level > 0) {
                threat_count++;
                }


                manualFire(uc[i], coords);



                if (threat_count > 0) {
                inactivityTimer.reset();
                }




            }


            } // for





            // restart
            if (inactivityTimer.isTimeOut()) {

                const std::vector<UnitBase*>& uct = UnitInterface::getPlayerUnits(playerIndex);
                unsigned char moving_count = 0;

                for ( Uint8 i = 0; i < uct.size(); ++i)
                {
                if (uct[i]->unit_state.moving == true) {
                moving_count++;
                }
                }

                if (moving_count == 0) {
                touch_flag = false;
                }

            }


            // keep in touch
            if (touchTimer.isTimeOut()) {
            if (touch_flag == false) {
            UnitBase *more_enemyUnit;
            if (UnitInterface::queryClosestEnemyUnit(&more_enemyUnit,
                        unitc->unit_state.location, playerIndex))
            {

            doGoThere(more_enemyUnit->unit_state.location);
            touch_flag = true;
            touchTimer.reset();
            idle_units = false;
            }
            }


            }
            //



          }





          } // if end

}





//-----------------------------------------------------------------

    void
BotPlayer::processEvents3() //
{
}

//-----------------------------------------------------------------

    void
BotPlayer::processEvents4() //
{
}

//-----------------------------------------------------------------

    void
BotPlayer::processEvents5() //
{
}

//-----------------------------------------------------------------

    void
BotPlayer::processEvents6() //
{
}

//----------------------------------------------------------------------------------

    void
BotPlayer::processEvents7()  //
{
}

//-----------------------------------------------------------------------------

    void
BotPlayer::processEvents8() //
{
}

//------------------------------------------------------------------

    void
BotPlayer::processEvents9() //
{
}

//------------------------------------------------------------------

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

            //m_timer.changePeriod(5.0);
            int unitCount = UnitInterface::getUnitCount(playerIndex);

            if (unitCount > 0) {
                float new_time = 3.0 / unitCount;
                if (new_time > 1) {
                    new_time = 1;
                }
                m_timer.changePeriod(new_time);
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
BotPlayer::getSoRandomUnit(PlayerID playerIndex)
{
    const std::vector<UnitBase*>& units
        = UnitInterface::getPlayerUnits(playerIndex);

    if(units.size() == 0) {
        return 0;
    } else {

    //size_t unitIndex = rand() % units.size();



    size_t unitIndex = rand() % units.size();
    UnitBase* chosen_unit = units[unitIndex];

    //unitsm.clear();
    return chosen_unit;
    /*
    if (units[unitIndex]->unit_state.lifecycle_state == _UNIT_LIFECYCLE_ACTIVE) {
    return units[unitIndex];
    }
    */

    }

    return 0;

}

//---------------------------------------------------------------------


//----------------------------------------------------------


UnitBase *
BotPlayer::getRandomUnit(PlayerID playerIndex)
{
    const std::vector<UnitBase*>& units
        = UnitInterface::getPlayerUnits(playerIndex);

    if(units.size() == 0)
        return 0;

    size_t unitIndex = rand() % units.size();
    //if (units[unitIndex]->unit_state.lifecycle_state != _UNIT_LIFECYCLE_INACTIVE && units[unitIndex]->unit_state.lifecycle_state != _UNIT_LIFECYCLE_DESTROYED) {
    return units[unitIndex];
    //} else {
    //return 0;
   //}

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
        if (  ObjectiveInterface::getObjective(i)->occupying_player == PlayerInterface::getLocalPlayer() )
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


static Objective* getRandomEnemyOrFreeOutpostNA()
{
    ObjectiveID num_objectives = ObjectiveInterface::getObjectiveCount();
    outpostList_t outposts;
    for ( int i = 0; i < num_objectives; ++i )
    {

        if ( ! ObjectiveInterface::getObjective(i)->occupying_player )
        {
            outposts.push_back(i);
        }
        else if ( ObjectiveInterface::getObjective(i)->occupying_player->getID() != PlayerInterface::getLocalPlayerIndex() &&
                    ! PlayerInterface::isAllied(ObjectiveInterface::getObjective(i)->occupying_player->getID(), PlayerInterface::getLocalPlayerIndex()) )
        {
            outposts.push_back(i);
        }
        else if ( ObjectiveInterface::getObjective(i)->occupying_player->getID() != PlayerInterface::getLocalPlayerIndex() ) {
           outposts.push_back(i);
        }
    }

    if ( ! outposts.empty() )
    {
        return ObjectiveInterface::getObjective(outposts[rand()%outposts.size()]);
    }



    return 0;
}



static Objective* getRandomEnemyNA()
{
    ObjectiveID num_objectives = ObjectiveInterface::getObjectiveCount();
    outpostList_t outposts;
    for ( int i = 0; i < num_objectives; ++i )
    {

        if ( ! ObjectiveInterface::getObjective(i)->occupying_player )
        {
            // none
        }
        else if ( ObjectiveInterface::getObjective(i)->occupying_player->getID() != PlayerInterface::getLocalPlayerIndex() &&
                    ! PlayerInterface::isAllied(ObjectiveInterface::getObjective(i)->occupying_player->getID(), PlayerInterface::getLocalPlayerIndex()) )
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
        outpost = getRandomEnemyOrFreeOutpostNA();
    }

    if (outpost)
    {
        // XXX hack, const occupation_pad_offset
        static const iXY occupation_pad_offset(240, 38); // 224, 48

        iXY map_loc;
        MapInterface::pointXYtoMapXY(
                outpost->location + occupation_pad_offset,
                map_loc);
        moveUnit(unit, map_loc);
    }
}

void
BotPlayer::unitOccupyOupostNA(UnitBase *unit)
{

    Objective *outpost = getRandomEnemyOrFreeOutpostNA();

    if (outpost)
    {
        // XXX hack, const occupation_pad_offset
        static const iXY occupation_pad_offset(240, 38); // 224, 48

        iXY map_loc;
        MapInterface::pointXYtoMapXY(
                outpost->location + occupation_pad_offset,
                map_loc);
        moveUnit(unit, map_loc);
    } else {



    }
}

void
BotPlayer::unitOccupyEnemyOupostNA(UnitBase *unit)
{

    Objective *outpost = getRandomEnemyNA();

    if (outpost)
    {
        // XXX hack, const occupation_pad_offset
        static const iXY occupation_pad_offset(240, 38); // 224, 48

        iXY map_loc;
        MapInterface::pointXYtoMapXY(
                outpost->location + occupation_pad_offset,
                map_loc);
        moveUnit(unit, map_loc);
    } else {

    Objective *spare_outpost = getRandomEnemyOrFreeOutpost();
    if (spare_outpost)
    {
        // XXX hack, const occupation_pad_offset
        static const iXY occupation_pad_offset(240, 38); // 224, 48

        iXY map_loc;
        MapInterface::pointXYtoMapXY(
                spare_outpost->location + occupation_pad_offset,
                map_loc);
        moveUnit(unit, map_loc);
    }



    }





}

//-----------------------------------------------------------------


void
BotPlayer::goThere(UnitBase *unit, iXY dest_loc)
{

        iXY map_loc;
        MapInterface::pointXYtoMapXY(dest_loc, map_loc);
        moveUnit(unit, map_loc);

}

void
BotPlayer::doGoThere(iXY dest_loc)
{

        iXY map_loc = dest_loc;
        //LOGGER.info("dest is %d,%d", dest_loc.x, dest_loc.y);

        sendMoveCommand(map_loc);

}





void BotPlayer::orientationToOffset( unsigned short orientation, signed char *offset_x, signed char *offset_y )
{
    switch ( orientation )
    {
        case 0:
            *offset_x =  1; *offset_y =  0;
            break;
        case 1:
            *offset_x =  1; *offset_y = -1;
            break;
        case 2:
            *offset_x =  0; *offset_y = -1;
            break;
        case 3:
            *offset_x = -1; *offset_y = -1;
            break;
        case 4:
            *offset_x = -1; *offset_y =  0;
            break;
        case 5:
            *offset_x = -1; *offset_y =  1;
            break;
        case 6:
            *offset_x =  0; *offset_y =  1;
            break;
        case 7:
            *offset_x =  1; *offset_y =  1;
            break;
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


void
BotPlayer::outpostProduceZero()
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
            produceUnit( i, 1 );
        }
    }
}



void BotPlayer::disownRandomOwned()
{
    ObjectiveID num_objectives = ObjectiveInterface::getObjectiveCount();
    outpostList_t outposts;
    for ( int i = 0; i < num_objectives; ++i )
    {


        if ( ! ObjectiveInterface::getObjective(i)->occupying_player )
        {
            // none
        }
        else if ( ObjectiveInterface::getObjective(i)->occupying_player->getID() == PlayerInterface::getLocalPlayerIndex() )
        {
            outposts.push_back(i);
        }

    }

    if ( ! outposts.empty() )
    {

        ObjectiveInterface::sendDisownObj(outposts[rand()%outposts.size()], 1, PlayerInterface::getLocalPlayerIndex());

    }

}






