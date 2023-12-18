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
#ifndef _PLAYERUNITCONFIG_HPP
#define _PLAYERUNITCONFIG_HPP

#include "Units/UnitTypes.hpp"

#include <vector>

class PlayerUnitConfig
{
protected:
    unsigned int max_allowed_units;
    std::vector<unsigned int> unit_spawn_list;
    char unit_color;

public:
    PlayerUnitConfig();

    void initialize();

    inline unsigned int getSpawnUnitCount( unsigned char unit_type ) const
    {
        if ( unit_type < unit_spawn_list.size() )
            return unit_spawn_list[unit_type];
        return 0;
    }

    inline void incrementSpawnUnitCount( unsigned char unit_type )
    {
        if ( unit_type < unit_spawn_list.size() )
        {
            unit_spawn_list[ unit_type ]++;
            if ( unitTotal() > max_allowed_units )
            {
                unit_spawn_list[ unit_type ]--;
            }
        }
    }

    inline void decrementSpawnUnitCount( unsigned char unit_type )
    {
        if ( unit_type < unit_spawn_list.size() )
        {
            if ( (unit_spawn_list[ unit_type ] > 0) &&
                 (unitTotal() > 1) )
            {
                unit_spawn_list[ unit_type ]--;
            }
        }
    }

    inline unsigned int getMaxAllowedUnits( void ) const
    {
        return( max_allowed_units );
    }

    unsigned int unitTotal( void );

    inline char getUnitColor( void ) const
    {
        return( unit_color );
    }

    inline void setUnitColor( char color )
    {
        unit_color = color;
    }
};


#endif // ** _PLAYERUNITCONFIG_HPP
