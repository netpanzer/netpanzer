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
#ifndef _OBJECTIVE_HPP
#define _OBJECTIVE_HPP

#include "Core/CoreTypes.hpp"
#include "ArrayUtil/BoundBox.hpp"
#include "Util/Timer.hpp"

class PlayerState;
class ObjectiveSyncData;

class Objective
{
public:
    ObjectiveID   id;
    iRect         selection_box;
    unsigned char outpost_type;
    char          name[64];
    iXY           location;
    BoundBox      capture_area;
    BoundBox      area;
    PlayerState*  occupying_player;

// from outpost
    unsigned char outpost_state;
    iXY outpost_map_loc;
    iXY unit_generation_loc;
    iXY unit_collection_loc;
    iXY occupation_pad_offset;

    unsigned short unit_generation_type;
    bool unit_generation_on_flag;

    Timer occupation_status_timer;
    Timer unit_generation_timer;

    Objective(ObjectiveID ID, iXY location, BoundBox area );
    ~Objective() { }

    void changeOwner( PlayerState * new_owner );
    void changeUnitGeneration(bool is_on, int unit_type);

    void getSyncData( ObjectiveSyncData& sync_data );
    void syncFromData( const ObjectiveSyncData& sync_data );

    void updateStatus();



private:
    void attemptOccupationChange(PlayerState* player);

    void checkOccupationStatus( void );

    void generateUnits( void );


};

#endif // ** _OBJECTIVE_HPP
