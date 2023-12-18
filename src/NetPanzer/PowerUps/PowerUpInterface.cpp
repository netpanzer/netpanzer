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

#include "PowerUps/PowerUpInterface.hpp"

#include <stdlib.h>


#include "Interfaces/GameConfig.hpp"

#include "Interfaces/MapInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"

#include "PowerUps/BonusUnitPowerUp.hpp"
#include "PowerUps/UnitPowerUp.hpp"
#include "PowerUps/UnitGlobalPowerUp.hpp"
#include "PowerUps/EnemyRadarPowerUp.hpp"

#include "Util/Log.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/PowerUpNetMessage.hpp"

PowerUpInterface::PowerUpList PowerUpInterface::powerup_list;

int PowerUpInterface::power_up_limit;
int PowerUpInterface::power_up_regen_time_upper_bound = 300;
int PowerUpInterface::power_up_regen_time_lower_bound = 60;

Timer PowerUpInterface::regen_timer;

enum { _powerup_unit,
       _powerup_bonus_units,
       _powerup_global_unit
     };

int  powerup_probability_table[3] = { _powerup_unit,
                                      _powerup_bonus_units,
                                      _powerup_global_unit
                                    };

void PowerUpInterface::setPowerUpLimits(unsigned long map_size_x,
                                        unsigned long map_size_y )
{
    int active_players;

    active_players = PlayerInterface::getActivePlayerCount();

    // Magic Number 0.10 = 1/10
    int player_factor = int(( 0.10 ) * active_players);

    // Magic Number 0.0000625 = 1/16000
    int power_up_limit = int( ( 0.0000625 ) * ( (map_size_x * map_size_y) ) );
    power_up_limit = power_up_limit + (power_up_limit * player_factor);
    PowerUpInterface::power_up_limit = power_up_limit;
    power_up_regen_time_upper_bound =  300;
    power_up_regen_time_lower_bound =  60;
}

PowerUpID
PowerUpInterface::getNextPowerUpID()
{
    static PowerUpID nextid = 0;
    return nextid++;
}

void PowerUpInterface::generatePowerUp()
{
    unsigned long map_size_x, map_size_y;
    PowerUp *power_up = 0;
    float next_regen_interval;
    PowerUpCreateMesg create_mesg;
    iXY loc;

    while (powerup_list.size() < (size_t) power_up_limit)
    {

        //LOGGER.info("Generating here!!!  plimit is=%d", power_up_limit);
        map_size_x = MapInterface::getWidth();
        map_size_y = MapInterface::getHeight();

        loc.x = rand() % map_size_x;
        loc.y = rand() % map_size_y;
        if ( MapInterface::getMovementValue( loc ) == 0xFF )
        {
            continue; // no powerup, try next time.
        }
        //int prob_table_index;
        int powerup_type;

        int prob_table_index = rand() % 100;
        if (prob_table_index < 30) {
            prob_table_index = 1;
        } else if (prob_table_index >= 30 && prob_table_index < 80) {
            prob_table_index = 0;
        } else {
            prob_table_index = 2;
        }

        //prob_table_index = 2; //only global
        powerup_type = powerup_probability_table[ prob_table_index ];


        switch( powerup_type )
        {
        case _powerup_bonus_units :
            power_up = new BonusUnitPowerUp( loc, powerup_type );
            break;
/*
        case _powerup_bonus_units_more :
            power_up = new BonusUnitPowerUp( loc, powerup_type );
            break;
*/
        case _powerup_unit :
            power_up = new UnitPowerUp( loc, powerup_type );
            break;

        case _powerup_global_unit :
            power_up = new UnitGlobalPowerUp( loc, powerup_type );
            break;

/*
        case _powerup_enemy_radar :
            power_up = new EnemyRadarPowerUp( loc, powerup_type );
            break;

        case _powerup_enemy_radar_more :
            power_up = new EnemyRadarPowerUp( loc, powerup_type );
            break;
*/
        default:
            LOGGER.info("Unknown powerup type?!?");
            return;
        }

        power_up->ID = getNextPowerUpID();

        powerup_list.push_back(power_up);

        create_mesg.set( power_up->map_loc,
                         power_up->ID,
                         power_up->type
                       );
        //LOGGER.info("broadcasting  plsize=%d id=%d type=%d", powerup_list.size(), power_up->ID, power_up->type);
        SERVER->broadcastMessage(&create_mesg, sizeof(create_mesg));

        int regrange = power_up_regen_time_upper_bound - power_up_regen_time_lower_bound;
        next_regen_interval = rand() % regrange;
        next_regen_interval += power_up_regen_time_lower_bound;
        regen_timer.changePeriod( next_regen_interval );

        setPowerUpLimits( map_size_x, map_size_y );
    }
}

void PowerUpInterface::initialize( void )
{
    PowerUp::POWERUP_ANIM.load( "powerups/Bolt.pak" );
    PowerUp::POWERUP_ANIM.setFPS( 15 );
    PowerUp::POWERUP_ANIM_SHADOW.load( "powerups/BoltS.pak" );

    PowerUp::POWERUP_ANIM_R.load( "powerups/Bolt_R.pak" );
    PowerUp::POWERUP_ANIM_R.setFPS( 15 );
    PowerUp::POWERUP_ANIM_SHADOW_R.load( "powerups/BoltS_R.pak" );
}

void PowerUpInterface::uninitialize( void )
{
    PowerUp::POWERUP_ANIM.unload( "powerups/Bolt.pak" );
    PowerUp::POWERUP_ANIM_SHADOW.unload( "powerups/BoltS.pak" );
    PowerUp::POWERUP_ANIM_R.unload( "powerups/Bolt_R.pak" );
    PowerUp::POWERUP_ANIM_SHADOW_R.unload( "powerups/BoltS_R.pak" );
}


void PowerUpInterface::resetLogic( void )
{

    unsigned long map_size_x, map_size_y;

    //if( NetworkState::status == _network_state_server ) {
    if( GameConfig::game_powerups == false )
    {
        return;
    }
    //}
    // here memory leak, should delete the pointer to powerups in the list
    PowerUpList::iterator i;
    for(i=powerup_list.begin(); i!=powerup_list.end(); i++)
    {
        delete *i;
    }
    powerup_list.clear();
    //LOGGER.info("in reset now");
    map_size_x = MapInterface::getWidth();
    map_size_y = MapInterface::getHeight();

    setPowerUpLimits( map_size_x, map_size_y );

    regen_timer.changePeriod( power_up_regen_time_upper_bound );

    if ( NetworkState::status == _network_state_server )
    {
        generatePowerUp();
    }


}

void PowerUpInterface::updateState()
{
    if( GameConfig::game_powerups == false )
    {
        return;
    }

    if ( NetworkState::status == _network_state_server )
    {
        if( regen_timer.count() )
        {
            generatePowerUp();
        }
    }

    for (PowerUpList::iterator i = powerup_list.begin();
            i != powerup_list.end(); /* empty */)
    {
        PowerUp* powerup = *i;

        if(powerup->life_cycle_state ==
                _power_up_lifecycle_state_inactive)
        {
            delete powerup;
            i = powerup_list.erase(i);
        }
        else
        {
            powerup->updateState();
            i++;
        }
    }
 }

void PowerUpInterface::offloadGraphics( SpriteSorter &sorter )
{
    if( GameConfig::game_powerups == false )
    {
        return;
    }

    PowerUpList::iterator i;
    for(i=powerup_list.begin(); i!=powerup_list.end(); i++)
    {
        PowerUp* powerup = *i;
        powerup->offloadGraphics(sorter);
    }
}

void PowerUpInterface::netMessagePowerUpCreate(const NetMessage* message)
{
    PowerUp *power_up = 0;
    PowerUpCreateMesg *create_mesg;

    create_mesg = (PowerUpCreateMesg *) message;

    //LOGGER.info("Got 1 Pwp Create msg!!! size is %d id=%d type=%d x=%d, y=%d", powerup_list.size(), create_mesg->getID(), create_mesg->getType(), create_mesg->getLocX(), create_mesg->getLocY());


    switch( create_mesg->getType() )
    {
    case _powerup_bonus_units :
        power_up = new BonusUnitPowerUp(
            iXY(create_mesg->getLocX(), create_mesg->getLocY()),
            _powerup_bonus_units );
        break;
/*
    case _powerup_bonus_units_more :
        power_up = new BonusUnitPowerUp(
            iXY(create_mesg->getLocX(), create_mesg->getLocY()),
            _powerup_bonus_units );
        break;
*/
    case _powerup_unit :
        power_up = new UnitPowerUp(
            iXY(create_mesg->getLocX(), create_mesg->getLocY()),
            _powerup_unit );
        break;

    case _powerup_global_unit :
        power_up = new UnitGlobalPowerUp(
            iXY(create_mesg->getLocX(), create_mesg->getLocY()),
            _powerup_global_unit );
        break;
/*
    case _powerup_enemy_radar :
        power_up = new EnemyRadarPowerUp(
            iXY(create_mesg->getLocX(), create_mesg->getLocY()),
            _powerup_enemy_radar);
        break;

    case _powerup_enemy_radar_more :
        power_up = new EnemyRadarPowerUp(
            iXY(create_mesg->getLocX(), create_mesg->getLocY()),
            _powerup_enemy_radar);
        break;
*/
    default:
        LOGGER.info("Unknown powerup type?!?");
        return;
    }

    power_up->ID = create_mesg->getID();

    powerup_list.push_back(power_up);
}

void PowerUpInterface::netMessagePowerUpHit(const NetMessage* message)
{
    PowerUpHitMesg *hit_mesg = (PowerUpHitMesg *) message;

    PowerUpList::iterator i;
    for ( i = powerup_list.begin(); i != powerup_list.end(); i++)
    {
        if ( (*i)->ID == hit_mesg->getID() )
        {
            (*i)->onHitMessage( hit_mesg );
            break;
        }
    }
}

void PowerUpInterface::processNetMessages(const NetMessage* message )
{
    switch(message->message_id)
    {
    case _net_message_id_powerup_create:
        netMessagePowerUpCreate(message);
        break;

    case _net_message_id_powerup_hit:
        netMessagePowerUpHit(message);
        break;

    default:
        LOGGER.warning("Unknown PowerUpMessage type (id %d-%d)",
                       message->message_class, message->message_id);
        break;
    }
}

void PowerUpInterface::syncPowerUps( ClientSocket * client )
{
    PowerUpList::iterator i;
    for(i=powerup_list.begin(); i!=powerup_list.end(); i++)
    {
        PowerUp* powerup_ptr = *i;

        PowerUpCreateMesg create_mesg;
        create_mesg.set( powerup_ptr->map_loc,
                         powerup_ptr->ID,
                         powerup_ptr->type
                       );

        client->sendMessage( &create_mesg, sizeof(create_mesg));
    }
}

void PowerUpInterface::syncPowerUpsBC()
{
    PowerUpList::iterator i;
    for(i=powerup_list.begin(); i!=powerup_list.end(); i++)
    {
        PowerUp* powerup_ptr = *i;

        PowerUpCreateMesg create_mesg;
        create_mesg.set( powerup_ptr->map_loc,
                         powerup_ptr->ID,
                         powerup_ptr->type
                       );

        //client->sendMessage( &create_mesg, sizeof(create_mesg));
        SERVER->broadcastMessage(&create_mesg, sizeof(create_mesg));
    }
}
