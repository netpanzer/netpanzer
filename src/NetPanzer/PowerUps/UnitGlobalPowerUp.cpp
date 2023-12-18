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

#include "UnitGlobalPowerUp.hpp"

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
#include "Objectives/ObjectiveInterface.hpp"

#include "System/Sound.hpp"
#include "Classes/Network/NetworkServer.hpp"

#include "Util/Log.hpp"

enum { _unit_global_powerup_firepower,
       _unit_global_powerup_speed,
       _unit_global_powerup_repair,
       _unit_global_powerup_reload,
       _unit_global_powerup_destruct,
       _unit_powerup_enum_count };

UnitGlobalPowerUp::UnitGlobalPowerUp(iXY map_loc, int type)
    : PowerUp( map_loc, type )
{
    unit_powerup_type = rand() % _unit_powerup_enum_count;
      //unit_powerup_type = 1; //gr

}



void UnitGlobalPowerUp::powerUpGlobalFirePower( UnitState *unit_state, UnitID unit_id )
{
    unsigned short top_damage_factor = UnitProfileInterface::tsu_damage_factor;

    if ( NetworkState::status == _network_state_server ) // server only
    {
    UnitBase* unit = UnitInterface::getUnit(unit_id);

    std::vector<UnitBase*> unitlist = UnitInterface::getPlayerUnits(unit->player->getID());


    for(std::vector<UnitBase*>::iterator i = unitlist.begin(); i != unitlist.end(); ++i) {
        UnitBase* unitx = *i;
        UnitState& unit_state = unitx->unit_state;

        int percent = 20;
        unsigned short new_damage_factor = unit_state.damage_factor+
                           (unsigned short)((double)unit_state.damage_factor * ((double)percent/(double)100));

        if (new_damage_factor > top_damage_factor) {
            new_damage_factor = top_damage_factor;
        }

        unit_state.damage_factor = new_damage_factor;
    }
    }

    UnitModGDM umgdm(unit_id, top_damage_factor);

    SERVER->broadcastMessage(&umgdm, sizeof(umgdm));
 }

void UnitGlobalPowerUp::powerUpGlobalSpeed( UnitState *unit_state, UnitID unit_id )
{

    unsigned short top_speed_rate = UnitProfileInterface::tsu_speed_rate;

    if ( NetworkState::status == _network_state_server ) // server only
    {
    UnitBase* unit = UnitInterface::getUnit(unit_id);

    std::vector<UnitBase*> unitlist = UnitInterface::getPlayerUnits(unit->player->getID());


    for(std::vector<UnitBase*>::iterator i = unitlist.begin(); i != unitlist.end(); ++i) {
        UnitBase* unitx = *i;
        UnitState& unit_state = unitx->unit_state;

        int percent = 20;
        unsigned short new_speed_rate = unit_state.speed_rate+
                           (unsigned short)((double)unit_state.speed_rate * ((double)percent/(double)100));


        if (new_speed_rate > top_speed_rate) {
            new_speed_rate = top_speed_rate;
        }

        unit_state.speed_rate = new_speed_rate;
    }
    }

    UnitModGS umgs(unit_id, top_speed_rate);

    SERVER->broadcastMessage(&umgs, sizeof(umgs));
}

void UnitGlobalPowerUp::powerUpGlobalReload( UnitState *unit_state, UnitID unit_id)
{
    unsigned short top_reload_time = UnitProfileInterface::tsu_reload_time;

    if ( NetworkState::status == _network_state_server ) // server only
    {
    UnitBase* unit = UnitInterface::getUnit(unit_id);

    std::vector<UnitBase*> unitlist = UnitInterface::getPlayerUnits(unit->player->getID());


    for(std::vector<UnitBase*>::iterator i = unitlist.begin(); i != unitlist.end(); ++i) {
        UnitBase* unitx = *i;
        UnitState& unit_state = unitx->unit_state;

        int percent = 20;
        unsigned short new_reload_time = unit_state.reload_time-
                           (unsigned short)((double)unit_state.reload_time * ((double)percent/(double)100));

        if (new_reload_time < top_reload_time) {
            new_reload_time = top_reload_time;
        }


        unit_state.reload_time = new_reload_time;



    }
    }

    UnitModGRT ugrt(unit_id, top_reload_time);

    SERVER->broadcastMessage(&ugrt, sizeof(ugrt));
}

void UnitGlobalPowerUp::powerUpGlobalDestruct( UnitID unit_id )
{
    //UMesgSelfDestruct self_destruct;
    //self_destruct.setHeader( unit_id, _umesg_flag_unique );
    //UnitInterface::sendMessage( &self_destruct );
    UnitBase* unit = UnitInterface::getUnit(unit_id);
    UnitInterface::destroyPlayerUnits(unit->player->getID());
    ObjectiveInterface::disownPlayerObjectives(unit->player->getID());
}

void UnitGlobalPowerUp::powerUpGlobalRepair( UnitState *unit_state, UnitID unit_id)
{

     UnitBase* unit = UnitInterface::getUnit( unit_id );

     std::vector<UnitBase*> unitlist = UnitInterface::getPlayerUnits(unit->player->getID());



     UnitModGR umgr(unit_id);

     SERVER->broadcastMessage(&umgr, sizeof(umgr));

    if ( NetworkState::status == _network_state_server ) // server only
    {
     for(std::vector<UnitBase*>::iterator i = unitlist.begin(); i != unitlist.end(); ++i) {
        UnitBase* unitx = *i;
        UnitState& unit_state = unitx->unit_state;

        unit_state.hit_points = unit_state.max_hit_points;

        UpdateStateUnitOpcode update_state_opcode;

        update_state_opcode.setUnitID(unitx->id);
        update_state_opcode.setHitPoints(unit_state.max_hit_points);
        UnitInterface::sendOpcodeP( &update_state_opcode ); //necessary to update state (unit damage bar) in other clients

        }
    }

}


static const char * powerupTypeToString( int type )
{
    switch( type )
    {

    case _unit_global_powerup_firepower:
        return( "UNIT GLOBAL FIREPOWER" );

    case _unit_global_powerup_speed:
        return( "UNIT GLOBAL SPEED" );

    case _unit_global_powerup_repair:
        return( "UNIT GLOBAL REPAIR" );

    case _unit_global_powerup_reload:
        return( "UNIT GLOBAL RELOAD TIME" );

    case _unit_global_powerup_destruct:
        return( "UNIT GLOBAL DESTRUCT" );
    }
    return("");

}

void UnitGlobalPowerUp::onHit( UnitID unit_id )
{
    sound->playPowerUpSound();

    UnitBase* unit = UnitInterface::getUnit( unit_id );

    switch( unit_powerup_type )
    {

    case _unit_global_powerup_firepower:
        powerUpGlobalFirePower( &(unit->unit_state), unit_id );
        break;

    case _unit_global_powerup_speed:
        powerUpGlobalSpeed( &(unit->unit_state), unit_id );
        break;

    case _unit_global_powerup_repair:
        powerUpGlobalRepair( &(unit->unit_state), unit_id );
        break;

    case _unit_global_powerup_reload:
        powerUpGlobalReload( &(unit->unit_state), unit_id );
        break;

    case _unit_global_powerup_destruct:
        powerUpGlobalDestruct( unit_id );
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



void UnitGlobalPowerUp::onHitMessage( PowerUpHitMesg *message  )
{
    sound->playPowerUpSound();
    life_cycle_state = _power_up_lifecycle_state_inactive;

    if( PlayerInterface::getLocalPlayerIndex() == message->getPlayerID() )
    {
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "YOU GOT A %s POWERUP",
                                      powerupTypeToString( message->getUnitPowerupType() ) );
    }
}
