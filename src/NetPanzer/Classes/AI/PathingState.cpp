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

#include "PathingState.hpp"

float         PathingState::astar_gen_time = 0.0;
float	      PathingState::astar_gen_time_total = 0.0;
unsigned long PathingState::path_length = 0;
unsigned long PathingState::path_gen_count = 0;
unsigned long PathingState::update_gen_count = 0;
unsigned long PathingState::path_cache_hits = 0;
unsigned long PathingState::path_cache_misses = 0;


void PathingState::resetStatistics( void )
{
    astar_gen_time = 0;
    astar_gen_time_total = 0;
    path_length = 0;
    path_gen_count = 0;
    update_gen_count = 0;
    path_cache_hits = 0;
    path_cache_misses = 0;
}
