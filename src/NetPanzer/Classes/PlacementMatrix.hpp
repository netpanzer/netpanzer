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
#ifndef _PLACEMENTMATRIX_HPP
#define _PLACEMENTMATRIX_HPP

#include "Types/iXY.hpp"

class PlacementMatrix
{
protected:
    enum { _placement_state_base_case,
           _placement_state_top_run,
           _placement_state_right_run,
           _placement_state_bottom_run,
           _placement_state_left_run
         };

    unsigned char placement_state;
    long run_length;
    long run_counter;
    iXY current_offset;
    iXY current_loc;
    iXY ini_loc;

    bool verifyLocation( iXY &loc );

protected:
    //long level;
    //long direction;
    //iXY ini_loc;

public:
    void reset( iXY ini_map_loc );

    bool getNextEmptyLoc( iXY *loc );


};

#endif // ** _PLACEMENTMATRIX_HPP
