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

#include "SelectionList.hpp"
#include "Units/UnitInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Util/Log.hpp"
#include "Units/UnitBase.hpp"
#include "Interfaces/WorldViewInterface.hpp"
#include <algorithm>

bool SelectionList::selectUnit(iXY point)
{
    deselect();
    unit_list.clear();

    UnitInterface::queryUnitsAt(unit_list, point,
                                PlayerInterface::getLocalPlayerIndex(),
                                _search_player);

    select();
    if (unit_list.size() == 0)
        return false;
    
    resetUnitCycling();
    return true;
}

bool SelectionList::addUnit(iXY point)
{
    deselect();

    UnitInterface::queryUnitsAt(unit_list, point,
                                PlayerInterface::getLocalPlayerIndex(),
                                _search_player);

    select();
    if (unit_list.size() == 0)
        return false;
    
    resetUnitCycling();
    return true;
}


bool SelectionList::selectTarget(iXY point)
{
    deselect();
    unit_list.clear();

    UnitInterface::queryUnitsAt(unit_list, point,
                                PlayerInterface::getLocalPlayerIndex(),
                                _search_exclude_player);

    if (unit_list.size() == 0)
        return false;
    
    resetUnitCycling();
    return true;
}

bool SelectionList::selectBounded(iRect bounds, bool addunits)
{
    std::vector<UnitID> tempunits;
    UnitInterface::queryUnitsAt(tempunits, bounds,
                                PlayerInterface::getLocalPlayerIndex(),
                                _search_player);
    
    if ( ! tempunits.size() )
        return false;
    
    if(!addunits)
    {
        deselect();
        unit_list.clear();
        unit_list = tempunits;
    }
    else
    {
        // have to verify that we don't put the already selected units again
        unit_list.insert(unit_list.end(), tempunits.begin(), tempunits.end());
        std::sort(unit_list.begin(), unit_list.end());
        std::unique(unit_list.begin(), unit_list.end());
    }
    
    select();
    
    resetUnitCycling();
    return true;
}

bool SelectionList::selectSameTypeVisible( iXY point, bool addunits)
{
    PlayerID player_id = PlayerInterface::getLocalPlayerIndex();

    if(!addunits) {
        deselect();
        unit_list.clear();
    }

    std::vector<UnitID> temp_list;

    UnitInterface::queryUnitsAt(temp_list, point, player_id, _search_player);
    
    if ( temp_list.empty() )
        return false;
        
    unsigned char t=UnitInterface::getUnit(temp_list[0])->unit_state.unit_type;
    temp_list.clear();
    
    iRect wr;
    WorldViewInterface::getViewWindow(&wr);
    UnitInterface::queryUnitsAt(temp_list, wr, player_id, _search_player);
    
    int p = temp_list.size();
    if ( !p )
        return false;

    p--;
    do {
        if ( UnitInterface::getUnit(temp_list[p])->unit_state.unit_type == t)
            unit_list.push_back(temp_list[p]);
    } while (p--);

    temp_list.clear();
    
    select();
    if (unit_list.size() == 0)
        return false;
    
    resetUnitCycling();
    return true;
}

void SelectionList::select()
{
    unsigned long id_list_index;
    unsigned long id_list_size;
    UnitBase *unit;

    std::sort(unit_list.begin(), unit_list.end());
    unit_list.erase(std::unique(unit_list.begin(), unit_list.end()), unit_list.end());
    
    id_list_size = unit_list.size();

    for( id_list_index = 0; id_list_index < id_list_size; id_list_index++ ) {
        unit = UnitInterface::getUnit( unit_list[ id_list_index ] );
        if ( unit != 0 ) {
            unit->unit_state.select = true;
        }
    }

}

void SelectionList::deselect( void )
{
    unsigned long id_list_index;
    unsigned long id_list_size;
    UnitBase *unit;

    id_list_size = unit_list.size();

    for( id_list_index = 0; id_list_index < id_list_size; id_list_index++ ) {
        unit = UnitInterface::getUnit( unit_list[ id_list_index ] );
        if ( unit != 0 ) {
            unit->unit_state.select = false;
        }
    }
}

void SelectionList::cycleNextUnit( void )
{
    UnitBase *unit;
    unsigned long start_index;

    start_index = unit_cycle_index;

    if ( unit_list.size() == 0 )
        return;

    deselect();
    do {

        unit = UnitInterface::getUnit( unit_list[ unit_cycle_index ] );
        if ( unit != 0 ) {
            unit->unit_state.select = true;
        }

        unit_cycle_index = (unit_cycle_index + 1) % unit_list.size();

    } while( (unit == 0) && (unit_cycle_index != start_index) );
}

void SelectionList::copyList( SelectionList &source_list )
{
    unGroup();

    addList(source_list);
}

void SelectionList::addList( SelectionList &source_list )
{
    unsigned long list_index;
    unsigned long list_size;

    list_size=source_list.unit_list.size();

    for ( list_index = 0; list_index < list_size; list_index++ ) {
        unit_list.push_back(source_list.unit_list[ list_index ]);
    }

    unit_cycle_index = source_list.unit_cycle_index;

    resetUnitCycling();
}

unsigned short SelectionList::getHeadUnitType()
{
    UnitBase *unit;

    if ( unit_list.size() > 0 ) {
        unit = UnitInterface::getUnit( unit_list[ 0 ] );
        if( unit != 0 ) {
            return unit->unit_state.unit_type;
        } else {
            return 0;
        }
    }

    return 0;
}

void SelectionList::validateList()
{
    unsigned long id_list_index;
    unsigned long id_list_size;
    UnitBase *unit;

    id_list_size = unit_list.size();

    if( id_list_size == 0) {
        return;
    }

    for( id_list_index = 0; id_list_index < id_list_size; id_list_index++ ) {
        unit = UnitInterface::getUnit( unit_list[ id_list_index ] );
        if ( unit != 0 ) {
            return;
        }
    }

    unit_list.clear();
}

