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

#include "UnitPowerUp.hpp"

#include <stdlib.h>
#include "Units/UnitTypes.hpp"
#include "Units/UnitInterface.hpp"
#include "Classes/UnitMessageTypes.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"

#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/UnitNetMessage.hpp"
#include "Classes/Network/PowerUpNetMessage.hpp"

#include "Units/UnitProfileInterface.hpp"

#include "System/Sound.hpp"
#include "Classes/Network/NetworkServer.hpp"

#include "Util/Log.hpp"

enum { //_unit_powerup_hitpoints,
       //_unit_powerup_range,
       _unit_powerup_firepower,
       _unit_powerup_speed,
       //_unit_powerup_repair,
       _unit_powerup_reload,
       _unit_powerup_destruct,
       //_unit_powerup_global_repair,
       _unit_powerup_superunit,
       _unit_powerup_enum_count
     };

UnitPowerUp::UnitPowerUp(iXY map_loc, int type)
    : PowerUp( map_loc, type )
{
    unit_powerup_type = rand() % _unit_powerup_enum_count;
      //unit_powerup_type = 1; //superunit
      //unit_powerup_type = 3; // speed
}

void UnitPowerUp::powerUpHitPoints( UnitState *unit_state, UnitID unit_id )
{
    /*
    //int percent = 50;
    //short new_hit_points = unit_state->hit_points+
    //                       (short)((double)unit_state->hit_points * ((double)percent/(double)100));
    short new_hit_points = UnitProfileInterface::tsu_hit_points;
    //if (new_hit_points > UnitProfileInterface::tsu_hit_points) {
    //    new_hit_points = UnitProfileInterface::tsu_hit_points;
    //}
    UnitModHP umhp(unit_id, new_hit_points);
    unit_state->max_hit_points = new_hit_points;
    unit_state->hit_points = new_hit_points;
    //unit_state->max_hit_points = new_hit_points;
    SERVER->broadcastMessage(&umhp, sizeof(umhp));
    */
}
/*
void UnitPowerUp::powerUpWRange( UnitState *unit_state, UnitID unit_id )
{
    int percent = 50;
    unsigned long new_weapon_range = unit_state->weapon_range+
                           (unsigned long)((double)unit_state->weapon_range * ((double)percent/(double)100));
    if (new_weapon_range > UnitProfileInterface::tsu_weapon_range) {
        new_weapon_range = UnitProfileInterface::tsu_weapon_range;
    }
    UnitModWRange umwr(unit_id, new_weapon_range);
    unit_state->weapon_range = new_weapon_range;

    SERVER->broadcastMessage(&umwr, sizeof(umwr));
}
*/
void UnitPowerUp::powerUpFirePower( UnitState *unit_state, UnitID unit_id )
{
    int percent = 50;
    unsigned short new_damage_factor = unit_state->damage_factor+
                           (unsigned short)((double)unit_state->damage_factor * ((double)percent/(double)100));
    if (new_damage_factor > UnitProfileInterface::tsu_damage_factor) {
        new_damage_factor = UnitProfileInterface::tsu_damage_factor;
    }
    UnitModFire umf(unit_id, new_damage_factor);
    if ( NetworkState::status == _network_state_server ) // server only
    {
    unit_state->damage_factor = new_damage_factor;
    }
    SERVER->broadcastMessage(&umf, sizeof(umf));
 }

void UnitPowerUp::powerUpSpeed( UnitState *unit_state, UnitID unit_id )
{   /*
    int percent = 50;
    unsigned short new_speed_rate = unit_state->speed_rate+
                           (unsigned short)((double)unit_state->speed_rate * ((double)percent/(double)100));
    int new_speed_factor = unit_state->speed_factor;
    if (new_speed_rate*new_speed_factor > UnitProfileInterface::tsu_speed) {
    */
        unsigned short new_speed_rate = UnitProfileInterface::tsu_speed_rate;
        unsigned short new_speed_factor = UnitProfileInterface::tsu_speed_factor;
    //}
    UnitModSpeed ums(unit_id, new_speed_rate, new_speed_factor);
    if ( NetworkState::status == _network_state_server ) // server only
    {
    unit_state->speed_rate = new_speed_rate;
    unit_state->speed_factor = new_speed_factor;
    }
    SERVER->broadcastMessage(&ums, sizeof(ums));
}

void UnitPowerUp::powerUpRepair( UnitState *unit_state, UnitID unit_id)
{
    short max_hit_points = unit_state->max_hit_points;
    UnitModMHP ummhp(unit_id, max_hit_points);
    unit_state->hit_points = unit_state->max_hit_points;
    SERVER->broadcastMessage(&ummhp, sizeof(ummhp));
}

void UnitPowerUp::powerUpReload( UnitState *unit_state, UnitID unit_id)
{
    int percent = 50;
    unsigned short new_reload_time;

    if (unit_state->reload_time > 1) {
    new_reload_time = unit_state->reload_time-
                           (unsigned short)((double)unit_state->reload_time * ((double)percent/(double)100));

    if (new_reload_time < UnitProfileInterface::tsu_reload_time) {
        new_reload_time = UnitProfileInterface::tsu_reload_time;
       }

    } else {
    new_reload_time = 1;
    }


    UnitModReload umr(unit_id, new_reload_time);
    if ( NetworkState::status == _network_state_server ) // server only
    {
    unit_state->reload_time = new_reload_time;
    }
    SERVER->broadcastMessage(&umr, sizeof(umr));
}

void UnitPowerUp::powerUpDestruct( UnitID unit_id )
{
    UMesgSelfDestruct self_destruct;
    self_destruct.setHeader( unit_id, _umesg_flag_unique );
    UnitInterface::sendMessage( &self_destruct );
}
/*
void UnitPowerUp::powerUpGRepair( UnitState *unit_state, UnitID unit_id)
{

     UnitBase* unit = UnitInterface::getUnit( unit_id );

     std::vector<UnitBase*> unitlist = UnitInterface::getPlayerUnits(unit->player->getID());

        //loop here
     for(std::vector<UnitBase*>::iterator i = unitlist.begin(); i != unitlist.end(); ++i) {
        UnitBase* unitx = *i;
        UnitState& unit_state = unitx->unit_state;

        unit_state.hit_points = unit_state.max_hit_points;

        }

    //short max_hit_points = unit_state->max_hit_points;
    UnitModGR umgr(unit_id);
    //unit_state->hit_points = unit_state->max_hit_points;
    SERVER->broadcastMessage(&umgr, sizeof(umgr));
}
*/
void UnitPowerUp::powerUpSuperunit( UnitState *unit_state, UnitID unit_id )
{
    // best speed
        unsigned short new_speed_rate = UnitProfileInterface::tsu_speed_rate;
        unsigned short new_speed_factor = UnitProfileInterface::tsu_speed_factor;
    // best reload
        unsigned short new_reload_time = UnitProfileInterface::tsu_reload_time;
    // best firepower
        unsigned short new_damage_factor = UnitProfileInterface::tsu_damage_factor;
    // higher weapon range
        int percent = 50;
        unsigned long new_weapon_range = unit_state->weapon_range+
                           (unsigned long)((double)unit_state->weapon_range * ((double)percent/(double)100));
    // and fully repaired of course
        short max_hit_points = unit_state->max_hit_points;
        //short max_hit_points = UnitProfileInterface::tsu_hit_points;

    UnitModSuperunit umsup(unit_id, new_speed_rate, new_speed_factor, new_reload_time, new_damage_factor, new_weapon_range, max_hit_points);
    if ( NetworkState::status == _network_state_server ) // server only
    {
    unit_state->speed_rate = new_speed_rate;
    //unit_state->speed_factor = new_speed_factor;
    unit_state->reload_time = new_reload_time;
    unit_state->damage_factor = new_damage_factor;
    unit_state->weapon_range = new_weapon_range;
    //unit_state->max_hit_points = max_hit_points;
    unit_state->hit_points = unit_state->max_hit_points;
    }
    SERVER->broadcastMessage(&umsup, sizeof(umsup));
}





static const char * powerupTypeToString( int type )
{
    switch( type )
    {
/*
    case _unit_powerup_hitpoints:
        return( "UNIT HITPOINTS" );

    case _unit_powerup_range:
        return( "UNIT WEAPON RANGE" );
*/
    case _unit_powerup_firepower:
        return( "UNIT FIREPOWER" );

    case _unit_powerup_speed:
        return( "UNIT SPEED" );
/*
    case _unit_powerup_repair:
        return( "UNIT REPAIR" );
*/
    case _unit_powerup_reload:
        return( "UNIT RELOAD TIME" );

    case _unit_powerup_destruct:
        return( "UNIT DESTRUCT" );
/*
    case _unit_powerup_global_repair:
        return( "GLOBAL UNIT REPAIR" );
*/
    case _unit_powerup_superunit:
        return( "SUPERUNIT" );
    }

    return("");
}

void UnitPowerUp::onHit( UnitID unit_id )
{
    sound->playPowerUpSound();

    UnitBase* unit = UnitInterface::getUnit( unit_id );

    switch( unit_powerup_type )
    {
        /*
    case _unit_powerup_hitpoints:
        powerUpHitPoints( &(unit->unit_state), unit_id );
        break;

    case _unit_powerup_range:
        powerUpWRange( &(unit->unit_state), unit_id );
        break;
        */
    case _unit_powerup_firepower:
        powerUpFirePower( &(unit->unit_state), unit_id );
        break;

    case _unit_powerup_speed:
        powerUpSpeed( &(unit->unit_state), unit_id );
        break;
        /*
    case _unit_powerup_repair:
        powerUpRepair( &(unit->unit_state), unit_id );
        break;
        */
    case _unit_powerup_reload:
        powerUpReload( &(unit->unit_state), unit_id );
        break;

    case _unit_powerup_destruct:
        powerUpDestruct( unit_id );
        break;
        /*
    case _unit_powerup_global_repair:
        powerUpGRepair( &(unit->unit_state), unit_id );
        break;
        */
    case _unit_powerup_superunit:
        powerUpSuperunit( &(unit->unit_state), unit_id );
        break;
    }

    PowerUpHitMesg hit_mesg;
    hit_mesg.set(ID, unit->player->getID(), unit_powerup_type);
    SERVER->broadcastMessage(&hit_mesg, sizeof(PowerUpHitMesg));

    life_cycle_state = _power_up_lifecycle_state_inactive;

    if(unit->player == PlayerInterface::getLocalPlayer())
    {
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "YOU GOT A %s POWERUP",
                                      powerupTypeToString( unit_powerup_type ) );
    }
}

void UnitPowerUp::onHitMessage( PowerUpHitMesg *message  )
{
    sound->playPowerUpSound();
    life_cycle_state = _power_up_lifecycle_state_inactive;

    if( PlayerInterface::getLocalPlayerIndex() == message->getPlayerID() )
    {
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "YOU GOT A %s POWERUP",
                                      powerupTypeToString( message->getUnitPowerupType() ) );
    }
}
