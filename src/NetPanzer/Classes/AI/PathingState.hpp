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
#ifndef _PATHINGSTATE_HPP
#define _PATHINGSTATE_HPP

class PathingState
{
public:
    static float         astar_gen_time;
    static float	        astar_gen_time_total;
    static unsigned long path_length;
    static unsigned long path_gen_count;
    static unsigned long update_gen_count;
    static unsigned long path_cache_hits;
    static unsigned long path_cache_misses;

    static void resetStatistics( void );

    static double getAverageAstarPathTime( void )
    {
        if ( (path_gen_count + update_gen_count) > 0 ) {
            return( astar_gen_time_total / ( (float) (path_gen_count + update_gen_count) ) );
        }

        return( 0 );
    }
};





#endif // ** _PATHINGSTATE_HPP
