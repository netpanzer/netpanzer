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
#ifndef _SELECTIONLIST_HPP
#define _SELECTIONLIST_HPP

#include "Units/UnitState.hpp"
#include "Units/UnitInterface.hpp"

class SelectionList
{
protected:
    unsigned long unit_cycle_index;

public:
    std::vector<UnitID> unit_list;

    SelectionList( )
        : unit_cycle_index(0)
    { }

    bool selectUnit( iXY point );

    bool addUnit( iXY point );

    bool selectTarget( iXY point );

    bool selectBounded(iRect bounds, bool addunits);
    bool selectSameTypeVisible(iXY point, bool addunits);

    bool isSelected()
    {
        return unit_list.size() > 0;
    }

    void select();

    void deselect();

    void unGroup()
    {
        deselect( );
        unit_list.clear();
    }

    unsigned short getHeadUnitType();

    inline void resetUnitCycling()
    {
        unit_cycle_index = 0;
    }

    void cycleNextUnit();

    void copyList( SelectionList &source_list );
    void addList( SelectionList &source_list );

    void validateList();
};

#endif  // ** _SELECTIONLIST_HPP
