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
#ifndef _UNITBLACKBOARD_HPP
#define _UNITBLACKBOARD_HPP

#include "ArrayUtil/BitArray.hpp"
#include "Types/iXY.hpp"

class UnitBlackBoard
{
protected:
    friend class Vehicle;
    friend class UnitInterface; // for unmark unit hack
    
    static BitArray unit_loc_map;

    static inline void markUnitLoc( iXY &unit_map_loc )
    {
        unit_loc_map.setBit( unit_map_loc.x, unit_map_loc.y );
    }

    static inline void unmarkUnitLoc( iXY &unit_map_loc )
    {
        unit_loc_map.clearBit( unit_map_loc.x, unit_map_loc.y );
    }

public:
    static void initializeBlackBoard( void );

    static void resetBlackBoard();

    static void updateUnitLocs( void );

    static inline bool unitOccupiesLoc( iXY &unit_map_loc )
    {
        return( unit_loc_map.getBit( unit_map_loc.x, unit_map_loc.y ) );
    }


};

#endif // ** _UNITBLACKBOARD_HPP
