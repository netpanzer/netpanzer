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

#include "Objective.hpp"

#include "Util/Log.hpp"

#include "Units/UnitInterface.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/GameConfig.hpp"

#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/UnitNetMessage.hpp"
#include "Classes/Network/ObjectiveNetMessage.hpp"
#include "Classes/UnitMessageTypes.hpp"

#include "Interfaces/GameManager.hpp"




Objective::Objective(ObjectiveID id, iXY location, BoundBox area)
{
    this->id = id;
    this->location = location;
    capture_area = area;
    occupying_player = 0;

    MapInterface::pointXYtoMapXY( location, outpost_map_loc );
    selection_box.max = location + iXY( 64, 32 );
    selection_box.min = location + iXY( -224, -128 );
    this->area.min = iXY( -400, -144 );
    this->area.max = iXY(  304,  240 );
    outpost_type = 0;

    unit_generation_type = 0;
    occupation_status_timer.changePeriod( 3 );
    unit_generation_timer.changePeriod( 1 );
    unit_collection_loc = outpost_map_loc + iXY( 13, 13 );
    unit_generation_loc = iXY( 1, 3 );
    occupation_pad_offset = iXY( 224, 48 );
    unit_generation_on_flag = false;



}

void
Objective::changeOwner( PlayerState * new_owner )
{
    if ( occupying_player )
    {
        occupying_player->decObjectivesHeld();
    }

    occupying_player = new_owner;

    if ( occupying_player )
    {
        occupying_player->incObjectivesHeld();

        ConsoleInterface::postMessage(Color::cyan, false, 0,
                                      "'%s' has been occupied by '%s'",
                                      name, new_owner->getName().c_str() );
    }

    unit_collection_loc = outpost_map_loc + iXY( 13, 13 );
    unit_generation_on_flag = false;
}

void
Objective::changeUnitGeneration(bool is_on, int unit_type)
{
    unit_generation_on_flag = is_on;
    if ( is_on )
    {

        UnitProfile *profile;
        if ( NetworkState::status == _network_state_server )  // server only
        {
        profile = UnitProfileInterface::getUnitProfile( unit_type );
        } else {
        profile = UnitProfileInterface::getUnitProfile( unit_type );
        }

        //LOGGER.info("unit_type = %d", unit_type);
        //LOGGER.info("unit_type = %d", unit_type*GameConfig::getUnitStylesNum());
        if ( profile )
        {
            unit_generation_type = unit_type;
            unit_generation_timer.changePeriod( (float) profile->regen_time );
        }
        else
        {
            unit_generation_on_flag = false;
        }
    }
}

void Objective::getSyncData(ObjectiveSyncData& sync_data)
{
    PlayerID player_id = INVALID_PLAYER_ID;
    if ( occupying_player != 0 )
    {
        player_id = occupying_player->getID();
    }

    sync_data.set(player_id);
}

void
Objective::syncFromData(const ObjectiveSyncData& sync_data)
{
    if ( sync_data.getPlayerId() >= PlayerInterface::getMaxPlayers() )
    {
        occupying_player = 0;
        if ( sync_data.getPlayerId() != INVALID_PLAYER_ID )
        {
            LOGGER.warning("Malformed ObjectvieMesgSync");
        }
    }
    else
    {
        occupying_player = PlayerInterface::getPlayer(sync_data.getPlayerId());
    }
}

void
Objective::attemptOccupationChange(PlayerState* player)
{
    if ( ! player->isActive() )
    {
        return;
    }

    if ( GameConfig::game_base_limit > 0 && player->getObjectivesHeld() >= GameConfig::game_base_limit )
    {
/*
        char base_limit_warning[140];
        sprintf(base_limit_warning, "Warning '%s' - Max %i bases per player!", player->getName().c_str(), GameConfig::game_base_limit);
        ChatInterface::serversayTo(player->getID(), base_limit_warning);
*/
        return; // cannot capture more bases.
    }

    changeOwner(player);

    ObjectiveOccupationUpdate msg;
    msg.set(id, player->getID());
    SERVER->broadcastMessage(&msg, sizeof(msg));
}

void
Objective::checkOccupationStatus()
{
    if ( occupation_status_timer.count() )
    {
        iRect bounding_area;

        if ( GameConfig::game_base_capture_mode == 1 )
        {
            iXY occupation_pad_loc(location);
            occupation_pad_loc += occupation_pad_offset;
            bounding_area = capture_area.getAbsRect( occupation_pad_loc );
        }
        else if ( GameConfig::game_base_capture_mode == 2 )
        {
            bounding_area = area.getAbsRect(location);
        }

        PlayerState *player = UnitInterface::querySinglePlayerInArea(bounding_area);
        if ( player && player != occupying_player )
        {
            attemptOccupationChange(player);
        }
    } // ** if occupation_status_timer.count()

}

void
Objective::generateUnits()
{   // XXX CHECK
    if ( occupying_player && unit_generation_on_flag == true )
    {
        if( unit_generation_timer.count() == true )
        {
            UnitBase *unit;
            iXY gen_loc;
            gen_loc = outpost_map_loc + unit_generation_loc;

            unit = UnitInterface::createUnit(unit_generation_type,
                    gen_loc, occupying_player->getID());

            if ( unit != 0 )
            {
                UnitRemoteCreate create_mesg(unit->player->getID(),
                        unit->id, gen_loc.x, gen_loc.y,
                        unit_generation_type);
                SERVER->broadcastMessage(&create_mesg, sizeof(UnitRemoteCreate));

                UMesgAICommand ai_command;
                PlacementMatrix placement_matrix;
                iXY collection_loc, loc;

                collection_loc = /*outpost_map_loc +*/ unit_collection_loc;

                placement_matrix.reset( collection_loc );
                placement_matrix.getNextEmptyLoc( &loc );

                ai_command.setHeader( unit->id, _umesg_flag_unique );
                ai_command.setMoveToLoc( loc );
                UnitInterface::sendMessage( &ai_command );
            }
        } // ** if
    } // ** if
}

void
Objective::updateStatus()
{
    if ( NetworkState::status == _network_state_server )
    {
        if ( GameConfig::game_base_capture_mode > 0 )
        {
            checkOccupationStatus();
        }
        generateUnits();
    }
    else
    {
        if( unit_generation_timer.count() == true )
        {
            unit_generation_timer.reset();
        }
    }
}
