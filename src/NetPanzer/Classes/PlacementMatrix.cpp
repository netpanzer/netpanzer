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

#include "Classes/PlacementMatrix.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Units/UnitBlackBoard.hpp"

void PlacementMatrix::reset( iXY ini_map_loc )
{
    ini_loc = ini_map_loc;
    current_offset = iXY( 0, 0 );
    current_loc =	ini_loc;
    run_length = 0;
    run_counter = 0;
    placement_state = _placement_state_base_case;
}

bool PlacementMatrix::verifyLocation( iXY &loc )
{
    long x_offset, y_offset;
    iXY succ;
    unsigned long direction_index;

    if ( ( MapInterface::getMovementValue( loc ) >= 0xFF) ||
            ( UnitBlackBoard::unitOccupiesLoc( loc ) == true )
       ) {
        return(false);
    }


    for ( direction_index = 0; direction_index < 9; direction_index++ ) {
        switch( direction_index ) {
        case 0: {
                x_offset =  0; y_offset =  0;
            }
            break;
        case 1: {
                x_offset =  1; y_offset =  0;
            }
            break;
        case 2: {
                x_offset =  1; y_offset = -1;
            }
            break;
        case 3: {
                x_offset =  0; y_offset = -1;
            }
            break;
        case 4: {
                x_offset = -1; y_offset = -1;
            }
            break;
        case 5: {
                x_offset = -1; y_offset =  0;
            }
            break;
        case 6: {
                x_offset = -1; y_offset =  1;
            }
            break;
        case 7: {
                x_offset =  0; y_offset =  1;
            }
            break;
        case 8: {
                x_offset =  1; y_offset =  1;
            }
            break;
        default:
            assert(false);
            return false;
        } // ** switch

        succ.x = loc.x + (x_offset);
        succ.y = loc.y + (y_offset);

        if ( ( MapInterface::getMovementValue( succ ) < 0xFF) &&
                ( UnitBlackBoard::unitOccupiesLoc( succ ) == false )
           ) {
            return true;
        }

    }

    return false;
}


bool PlacementMatrix::getNextEmptyLoc( iXY *loc )
{
    bool end_cycle = false;

    do {
        switch( placement_state ) {
        case _placement_state_base_case : {
                placement_state = _placement_state_top_run;
                current_offset.x -= 2;
                current_offset.y -= 2;
                current_loc = ini_loc + current_offset;
                run_length += 4;

                if ( verifyLocation( ini_loc ) == true ) {
                    *loc = ini_loc;
                    return( true );
                }

            }
            break;

        case _placement_state_top_run : {
                if ( run_counter >= run_length ) {
                    run_counter = 0;
                    placement_state = _placement_state_right_run;
                } else
                    if ( verifyLocation( current_loc ) == true ) {
                        *loc = current_loc;

                        run_counter += 2;
                        current_loc.x += 2;

                        return( true );
                    } else {
                        current_loc.x += 2;
                        run_counter += 2;
                    }
            }
            break;

        case _placement_state_right_run : {
                if ( run_counter >= run_length ) {
                    run_counter = 0;
                    placement_state = _placement_state_bottom_run;
                } else
                    if ( verifyLocation( current_loc ) == true ) {
                        *loc = current_loc;

                        run_counter += 2;
                        current_loc.y += 2;

                        return( true );
                    } else {
                        current_loc.y += 2;
                        run_counter += 2;
                    }
            }
            break;

        case _placement_state_bottom_run : {
                if ( run_counter >= run_length ) {
                    run_counter = 0;
                    placement_state = _placement_state_left_run;
                } else
                    if ( verifyLocation( current_loc ) == true ) {
                        *loc = current_loc;

                        run_counter += 2;
                        current_loc.x -= 2;

                        return( true );
                    } else {
                        current_loc.x -= 2;
                        run_counter += 2;
                    }
            }
            break;

        case _placement_state_left_run : {
                if ( run_counter >= run_length ) {
                    if ( current_offset.x > -50 ) {
                        run_counter = 0;
                        current_offset.x -= 2;
                        current_offset.y -= 2;
                        current_loc = ini_loc + current_offset;
                        run_length += 4;
                        placement_state = _placement_state_top_run;
                    } else {
                        return( false );
                    }
                } else
                    if ( verifyLocation( current_loc ) == true ) {
                        *loc = current_loc;

                        run_counter += 2;
                        current_loc.y -= 2;

                        return( true );
                    } else {
                        current_loc.y -= 2;
                        run_counter += 2;
                    }
            }
            break;


        } // ** switch
    } while( end_cycle == false );

    return( false );
}

/*
void PlacementMatrix::reset( iXY ini_map_loc )
 {
  *
  ini_loc = ini_map_loc;
  level = 2;
  direction = 0;
 }
 
bool PlacementMatrix::getNextEmptyLoc( iXY *loc )
 {
  long x_offset, y_offset;
  iXY succ;
  iXY succ_right;
  iXY succ_left;
 
  while ( level < 50 )
   {
    switch( direction )
     {
      case 0: { x_offset =  0; y_offset =  0; } break;
      case 1: { x_offset =  1; y_offset =  0; } break;
      case 2: { x_offset =  1; y_offset = -1; } break;
      case 3: { x_offset =  0; y_offset = -1; } break;
      case 4: { x_offset = -1; y_offset = -1; } break;
      case 5: { x_offset = -1; y_offset =  0; } break;
      case 6: { x_offset = -1; y_offset =  1; } break;
      case 7: { x_offset =  0; y_offset =  1; } break;
      case 8: { x_offset =  1; y_offset =  1; } break; 
     } // ** switch   
      
    succ.x = ini_loc.x + (x_offset * level);
    succ.y = ini_loc.y + (y_offset * level);
 
     switch( direction )
       {
        case 0: { succ_right.y = succ.y; 
                  succ_left.y = succ.y;
                  succ_right.x = succ.x;
                  succ_left.x = succ.x;  } break;
 
        case 1: { succ_right.y = succ.y + 1; 
                  succ_left.y = succ.y - 1;
                  succ_right.x = succ.x;
                  succ_left.x = succ.x;  } break;
        
        case 2: { succ_right.y = succ.y + 1;
                  succ_left.x = succ.x - 1;
                  succ_right.x = succ.x;
                  succ_left.y = succ.y;   } break;
        
        case 3: { succ_right.x = succ.x + 1; 
                  succ_left.x = succ.x - 1;
                  succ_right.y = succ.y;
                  succ_left.y = succ.y; } break;
                  
        case 4: { succ_right.x = succ.x + 1; 
                  succ_left.y = succ.y + 1;
                  succ_right.y = succ.y;
                  succ_left.x = succ.x; } break;
        
        case 5: { succ_right.y = succ.y - 1; 
                  succ_left.y = succ.y + 1;
                  succ_right.x = succ.x;
                  succ_left.x = succ.x; } break;
        
        case 6: { succ_right.y = succ.y - 1; 
                  succ_left.x = succ.x + 1;
                  succ_right.x = succ.x;
                  succ_left.y = succ.y; } break;
        
        case 7: { succ_right.x = succ.x - 1; 
                  succ_left.x = succ.x + 1;
                  succ_right.y = succ.y;
                  succ_left.y = succ.y; } break;
        
        case 8: { succ_right.x = succ.x + 1; 
                  succ_left.y = succ.y - 1;
                  succ_right.y = succ.y;
                  succ_left.x = succ.x; } break;
        
      } // ** switch    
 
   if ( ( MapInterface::getMovementValue( succ ) < 0xFF) &&
        ( UnitBlackBoard::unitOccupiesLoc( succ ) == false ) &&
		( (MapInterface::getMovementValue( succ_right ) < 0xFF) 
		   || (MapInterface::getMovementValue( succ ) < 0xFF) 
		)
      )
    {
     *loc = succ;
	 direction++;
	 if (direction == 9)
	  {
	   level++;
	   direction = 0;
	  }	   
	 return( true );
    }
  
   direction++;
   if (direction == 9)
    {
     level++;
     direction = 0;
    }	   
  
  } // ** while 
  
  return( false );
 }
 */
