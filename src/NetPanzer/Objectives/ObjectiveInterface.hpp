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
#ifndef _OBJECTIVEINTERFACE_HPP
#define _OBJECTIVEINTERFACE_HPP

#include "Core/CoreTypes.hpp"

class Objective;
class NetPacket;
class NetMessage;
class iXY;
class ClientSocket;

class ObjectiveInterface
{
protected:

    static Objective** objective_list;
    static int num_objectives;

    static void cleanUpObjectiveList();

public:
    static void cleanUp();

    static void resetLogic();

    static void loadObjectiveList( const char *file_path );

    static void serverHandleNetPacket(const NetPacket* packet);
    static void clientHandleNetMessage(const NetMessage* message);

    static void sendChangeGeneratingUnit(ObjectiveID objective_id, Uint8 unit_type, bool active);
    static void sendDisownObj(ObjectiveID objective_id, Uint8 disown_scope, PlayerID player_id);
    static void sendChangeOutputLocation(ObjectiveID objective_id, Uint32 map_x, Uint32 map_y);

    static void updateObjectiveStatus();

    static void disownPlayerObjectives(PlayerID player_id);
    static void disownPlayerObjective(ObjectiveID objective_id, PlayerID player_id);

    static Objective* getObjective( ObjectiveID objective_id )
    {
        return (objective_list && objective_id < num_objectives)
                ? objective_list[objective_id]
                : 0;
    }

    static Objective* getObjectiveAtWorldXY( const iXY& loc );

    static size_t getObjectiveCount()
    {
        return num_objectives;
    }

    static int getObjectiveLimit();

    static void syncObjectives( ClientSocket * client );

};


#endif //  ** _OBJECTIVEINTERFACE_HPP
