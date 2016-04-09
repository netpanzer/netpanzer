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

#include "Classes/PlayerUnitConfig.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Units/UnitProfileInterface.hpp"

#include "Util/StringUtil.hpp"

#include "Util/Log.hpp"

// for atoi for now
#include <stdlib.h>

PlayerUnitConfig::PlayerUnitConfig()
{
}

void PlayerUnitConfig::initialize()
{
    max_allowed_units = GameConfig::game_maxunits / GameConfig::game_maxplayers;

    int num_types = UnitProfileInterface::getNumUnitTypes();
    unit_spawn_list.resize(num_types, 0);
    
    int rem_units = max_allowed_units;
    int num_units;
    std::vector<NPString> str_counts;

    string_to_params(*GameConfig::game_unit_spawnlist, str_counts);

    for ( int n = 0; n < num_types ; n++ )
    {
        num_units = n < (int)str_counts.size() ? atoi(str_counts[n].c_str()) : 0;
        if ( num_units < 0 )
        {
            num_units = 0;
        }
        else if ( num_units > rem_units )
        {
            num_units = rem_units;
        }
        rem_units -= num_units;

        unit_spawn_list[n] = num_units;
    }
    
    if ( ! unitTotal() )
    {
        // there wasn't any unit, create some and add to config
        NPString cfg_str;
        for ( int a=0; a<(int)unit_spawn_list.size(); a++ )
        {
            unit_spawn_list[a] = 1;
            cfg_str += " 1";
        }
        GameConfig::game_unit_spawnlist->assign(cfg_str);
    }
    
    unit_color = 0;
}

unsigned int PlayerUnitConfig::unitTotal( void )
{
    unsigned int total = 0;
    unsigned int i;

    for( i = 0; i < unit_spawn_list.size(); i++ ) {
        total += unit_spawn_list[ i ];
    }

    return( total );
}
