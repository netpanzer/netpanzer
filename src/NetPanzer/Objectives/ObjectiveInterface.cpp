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

#include "ObjectiveInterface.hpp"
#include "Objective.hpp"

#include <stdio.h>
#include <memory>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "Interfaces/MapInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/GameConfig.hpp"

#include "Util/FileSystem.hpp"
#include "Util/Exception.hpp"
#include "Util/FileStream.hpp"
#include "Util/Log.hpp"

#include "Classes/Network/NetMessage.hpp"
#include "Classes/Network/ObjectiveNetMessage.hpp"
#include "Classes/Network/TerminalNetMesg.hpp"
#include "Classes/Network/NetPacket.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/NetMessageEncoder.hpp"

#include "Network/ClientSocket.hpp"

#include "Units/UnitProfileInterface.hpp"

Objective** ObjectiveInterface::objective_list = 0;
int ObjectiveInterface::num_objectives = 0;

void
ObjectiveInterface::cleanUpObjectiveList()
{
    if ( objective_list )
    {
        for ( int n=0; n < num_objectives; ++n )
        {
            delete objective_list[n];
        }
        delete[] objective_list;
        objective_list = 0;
    }

    num_objectives = 0;
}

void
ObjectiveInterface::cleanUp()
{
    cleanUpObjectiveList();
}

void
ObjectiveInterface::resetLogic()
{
    cleanUpObjectiveList();
}

static inline std::string readToken(std::istream& in, std::string tokenname)
{
    if(in.eof())
        throw std::runtime_error("file too short.");

    // skip whitespace characters and comments
    char c;
    do {
        in.get(c);
        if(c == '#') {  // comment till end of line
            do {
                in.get(c);
            } while(!in.eof() && c != '\n');
        }
    } while(!in.eof() && isspace(c));

    if(in.eof())
        throw std::runtime_error("file too short.");

    // read token
    std::string token;
    while(!in.eof() && !isspace(c)) {
        token += c;
        in.get(c);
    }
    if(token != tokenname) {
        std::stringstream msg;
        msg << "Expected token '" << tokenname << "' got '" << token << "'.";
        throw std::runtime_error(msg.str());
    }

    while(!in.eof() && isspace(c))
        in.get(c);

    // read token contents
    std::string result;
    while(!in.eof() && c != '\n') {
        result += c;
        in.get(c);
    }

    return result;
}

void
ObjectiveInterface::loadObjectiveList(const char *file_path)
{
    if ( ! GameConfig::game_enable_bases )
    {
        return;
    }

    ObjectiveID objective_count = 0;

    try
    {
        IFileStream in(file_path);

        cleanUpObjectiveList();

        std::string objectivecount = readToken(in, "ObjectiveCount:");
        std::stringstream ss(objectivecount);
        ss >> objective_count;

        if ( objective_count > 0 )
        {
            objective_list = new Objective*[objective_count];
            num_objectives = 0;
        }

        iXY loc;
        iXY world;
        std::string name;

        for (ObjectiveID objective_index = 0; objective_index < objective_count; objective_index++ )
        {
            Objective *objective_obj;

            name = readToken(in, "Name:");
            std::string location = readToken(in, "Location:");
            std::stringstream ss(location);
            ss >> loc.x >> loc.y;

            MapInterface::mapXYtoPointXY( loc, world );

            objective_obj = new Objective(objective_index, world,
                    BoundBox( -48, -32, 48, 32 )
                    );

            strcpy(objective_obj->name, name.c_str());
            objective_list[objective_index] = objective_obj;
            ++num_objectives;
        } // ** for
    }
    catch(std::exception& e)
    {
        cleanUpObjectiveList();
        LOGGER.warning("OILOL Error loading objectives '%s': %s. Working without them",
                       file_path, e.what());
    }
}

/**
 * Only used in server, when received a packet from a player.
 */
void
ObjectiveInterface::serverHandleNetPacket(const NetPacket* packet)
{
    const PlayerState* player = PlayerInterface::getPlayer(packet->fromPlayer);
    if ( !player )
    {
        LOGGER.warning("Couldn't find player for packet %u.",
                packet->fromPlayer);
        return;
    }

    const NetMessage* message = packet->getNetMessage();

    switch(message->message_id)
    {
        case _net_message_id_change_generating_unit:
        {
            const ObjectiveChangeGeneratingUnit* msg =
                (const ObjectiveChangeGeneratingUnit*) message;

            ObjectiveID obj_id = msg->getObjectiveId();
            if ( obj_id >= num_objectives )
            {
                LOGGER.warning("OISH_CGU CHEAT Player %u sent invalid objective_id %u, max is %u",
                               player->getID(), obj_id, num_objectives);
                break;
            }

            if ( objective_list[obj_id]->occupying_player != player )
            {
                LOGGER.warning("OISH_CGU CHEAT Player %u doesn't own objective %u",
                               player->getID(),
                               obj_id);
                break;
            }

            if ( msg->unit_type >= UnitProfileInterface::getNumUnitTypes() )
            {
                LOGGER.warning("OISH_CGU CHEAT Player %u sent invalid unit type %u, max is %u",
                               player->getID(),
                               msg->unit_type,
                               UnitProfileInterface::getNumUnitTypes());
                break;
            }

            objective_list[obj_id]->changeUnitGeneration(msg->unit_gen_on, msg->unit_type);

            if ( packet->fromClient )
            {
                packet->fromClient->sendMessage(msg, sizeof(ObjectiveChangeGeneratingUnit));
            }

            break;
        }

        case _net_message_id_change_output_location:
        {
            const ObjectiveChangeOutputLocation* msg =
                (const ObjectiveChangeOutputLocation*) message;

            ObjectiveID obj_id = msg->getObjectiveId();
            if ( obj_id >= num_objectives )
            {
                LOGGER.warning("OISH_COL CHEAT Player %u sent invalid objective_id %u, max is %u",
                               player->getID(), obj_id, num_objectives);
                break;
            }

            Uint32 map_x = msg->getMapX();
            Uint32 map_y = msg->getMapY();

            if ( map_x >= MapInterface::getWidth()
                 || map_y >= MapInterface::getHeight() )
            {
                LOGGER.warning("OISH_COL CHEAT Player %u sent invalid map location %u,%u; max is %u,%u",
                               player->getID(),
                               map_x, map_y,
                               (unsigned int)MapInterface::getWidth(),
                               (unsigned int)MapInterface::getHeight());
                break;
            }

            objective_list[obj_id]->unit_collection_loc.x = map_x;
            objective_list[obj_id]->unit_collection_loc.y = map_y;

            if ( packet->fromClient )
            {
                packet->fromClient->sendMessage(msg, sizeof(ObjectiveChangeOutputLocation));
            }

            break;
        }

        case _net_message_id_objective_disown:
        {
            const DisownObjective* msg =
                (const DisownObjective*) message;
            ObjectiveID obj_id = msg->getObjectiveId();
            if (msg->disown_scope == 0) {  // single disown

                if (objective_list[obj_id]->occupying_player == player && msg->getPlayerId() == player->getID()) {
                disownPlayerObjective(obj_id, msg->getPlayerId());
                }

            } else {  // multiple disown

                if (msg->getPlayerId() == player->getID()) {
                disownPlayerObjectives(msg->getPlayerId());
                }

            }

            break;
        }





        default:
            LOGGER.warning("OISH CHEAT Player %u sent unknown message type %u",
                           player->getID(),
                           message->message_id);
    }
}

void
ObjectiveInterface::clientHandleNetMessage(const NetMessage* message)
{
    switch(message->message_id)
    {
        case _net_message_id_occupation_status_update:
        {
            const ObjectiveOccupationUpdate* msg =
                (const ObjectiveOccupationUpdate*) message;

            ObjectiveID obj_id = msg->getObjectiveId();
            if ( obj_id >= num_objectives )
            {
                LOGGER.warning("OICH_OOU CHEAT SERVER sent invalid objective_id %u, max is %u",
                               obj_id, num_objectives);
                break;
            }

            PlayerID player_id = msg->getPlayerId();
            if ( player_id >= PlayerInterface::getMaxPlayers() && player_id != INVALID_PLAYER_ID )
            {
                LOGGER.warning("OICH_OOU CHEAT SERVER sent invalid player_id %u, max is %u",
                               player_id, PlayerInterface::getMaxPlayers());
                break;
            }

            PlayerState* player = 0;

            if ( player_id != INVALID_PLAYER_ID )
            {
                player = PlayerInterface::getPlayer(player_id);
                if ( player && ! player->isActive() )
                {
                    LOGGER.warning("OICH_OOU CHEAT SERVER sent inactive player_id %u",
                                   player_id);
                    break;
                }
            }

            objective_list[obj_id]->changeOwner(player);

            break;
        }

        case _net_message_id_objective_sync:
        {
            const ObjectiveSyncMesg* msg =
                (const ObjectiveSyncMesg*) message;

            ObjectiveID obj_id = msg->getObjectiveId();
            if ( obj_id >= num_objectives )
            {
                LOGGER.warning("OICH_OSM CHEAT SERVER sent invalid objective_id %u, max is %u",
                               obj_id, num_objectives);
                break;
            }

            objective_list[obj_id]->syncFromData(msg->sync_data);

            break;
        }

        case _net_message_id_change_generating_unit:
        {
            const ObjectiveChangeGeneratingUnit* msg =
                (const ObjectiveChangeGeneratingUnit*) message;

            ObjectiveID obj_id = msg->getObjectiveId();
            if ( obj_id >= num_objectives )
            {
                LOGGER.warning("OICH_CGU CHEAT SERVER sent invalid objective_id %u, max is %u",
                               obj_id, num_objectives-1);
                break;
            }

            if ( msg->unit_type >= UnitProfileInterface::getNumUnitTypes() )
            {
                LOGGER.warning("OICH_CGU CHEAT SERVER sent invalid unit type %u, max is %u",
                               msg->unit_type,
                               UnitProfileInterface::getNumUnitTypes());
                break;
            }

            objective_list[obj_id]->changeUnitGeneration(msg->unit_gen_on, msg->unit_type);

            break;
        }

        case _net_message_id_change_output_location:
        {
            const ObjectiveChangeOutputLocation* msg =
                (const ObjectiveChangeOutputLocation*) message;

            ObjectiveID obj_id = msg->getObjectiveId();
            if ( obj_id >= num_objectives )
            {
                LOGGER.warning("OICH_COL CHEAT SERVER sent invalid objective_id %u, max is %u",
                               obj_id, num_objectives-1);
                break;
            }

            Uint32 map_x = msg->getMapX();
            Uint32 map_y = msg->getMapY();

            if ( map_x >= MapInterface::getWidth()
                 || map_y >= MapInterface::getHeight() )
            {
                LOGGER.warning("OICH_COL CHEAT SERVER sent invalid map location %u,%u; max is %u,%u",
                               map_x, map_y,
                               (unsigned int)MapInterface::getWidth(),
                               (unsigned int)MapInterface::getHeight());
                break;
            }

            objective_list[obj_id]->unit_collection_loc.x = msg->getMapX();
            objective_list[obj_id]->unit_collection_loc.y = msg->getMapY();

            break;
        }

        default:
            LOGGER.warning("OICH CHEAT SERVER sent unknown message type %u",
                           message->message_id);
    }
}

void
ObjectiveInterface::sendChangeGeneratingUnit(ObjectiveID objective_id, Uint8 unit_type, bool active)
{
    if ( objective_id < num_objectives && unit_type < UnitProfileInterface::getNumUnitTypes() )
    {
        ObjectiveChangeGeneratingUnit msg;
        msg.set( objective_id, unit_type, active );
        CLIENT->sendMessage(&msg, sizeof(msg));
    }
}

void
ObjectiveInterface::sendDisownObj(ObjectiveID objective_id, Uint8 disown_scope, PlayerID player_id)
{
    if ( objective_id < num_objectives )
    {
        DisownObjective msg;
        msg.set( objective_id, disown_scope, player_id );
        CLIENT->sendMessage(&msg, sizeof(msg));
    }
}


void
ObjectiveInterface::sendChangeOutputLocation(ObjectiveID objective_id, Uint32 map_x, Uint32 map_y)
{
    if ( objective_id < num_objectives
         && map_x < MapInterface::getWidth()
         && map_y < MapInterface::getHeight() )
    {
        ObjectiveChangeOutputLocation msg;
        msg.set( objective_id, map_x, map_y );
        CLIENT->sendMessage(&msg, sizeof(msg));
    }
}

void
ObjectiveInterface::updateObjectiveStatus()
{
    for ( int i = 0; i < num_objectives; ++i )
    {
        objective_list[i]->updateStatus();
    }
}

void
ObjectiveInterface::disownPlayerObjectives(PlayerID player_id)
{
    for(Uint16 i = 0; i < num_objectives; ++i)
    {
        if ( objective_list[i]->occupying_player
             && objective_list[i]->occupying_player->getID() == player_id )
        {
            objective_list[i]->changeOwner(0);

            ObjectiveOccupationUpdate update_mesg;
            update_mesg.set( i, INVALID_PLAYER_ID);
            SERVER->broadcastMessage(&update_mesg, sizeof(update_mesg));
        }
    }
}

void
ObjectiveInterface::disownPlayerObjective(ObjectiveID objective_id, PlayerID player_id)
{

        if ( objective_list[objective_id]->occupying_player
             && objective_list[objective_id]->occupying_player->getID() == player_id )
        {
            objective_list[objective_id]->changeOwner(0);

            ObjectiveOccupationUpdate update_mesg;
            update_mesg.set( objective_id, INVALID_PLAYER_ID);
            SERVER->broadcastMessage(&update_mesg, sizeof(update_mesg));
        }

}

Objective*
ObjectiveInterface::getObjectiveAtWorldXY(const iXY& loc)
{
    for ( Uint16 i = 0; i < num_objectives; ++i )
    {
        if ( objective_list[i]->selection_box.contains( loc ) )
        {
            return objective_list[i];
        }
    }
    return 0;
}

void
ObjectiveInterface::syncObjectives( ClientSocket * client )
{
    NetMessageEncoder encoder;
    ObjectiveSyncMesg msg;

    for ( int i = 0; i < num_objectives; ++i )
    {
        msg.set(i);
        objective_list[i]->getSyncData( msg.sync_data );

        if ( ! encoder.encodeMessage(&msg, sizeof(msg)) )
        {
            client->sendMessage(encoder.getEncodedMessage(),
                                encoder.getEncodedLen());
            encoder.resetEncoder();
            encoder.encodeMessage(&msg, sizeof(msg)); // this time shuold be good
        }
    }

    client->sendMessage(encoder.getEncodedMessage(),
                        encoder.getEncodedLen());
}

int
ObjectiveInterface::getObjectiveLimit()
{
    float percentage
        = (float) GameConfig::game_occupationpercentage / 100.0;
    int wincount = (int) ( ((float) getObjectiveCount()) * percentage + 0.999);
    if(wincount == 0)
        wincount = 1;

    return wincount;
}

