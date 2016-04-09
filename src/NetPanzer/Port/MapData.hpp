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
#ifndef _MAPDATA_HPP
#define _MAPDATA_HPP

#define _MAX_SPAWN_LOCS 32

typedef
struct
{
    long spawn_x;  // x loc of player spawn
    long spawn_y;  // y loc of player spawn
    long spawn_type;  // type of spawn outpost / non outpost
}
spawn_loc_type;


typedef
struct
{
    char  pyro_id_header[64];   // file text identification;
    short pyro_map_id;

    char  map_name[64];
    short map_x_size;           // map x size in tiles
    short map_y_size;           // map y size in tiles

    char  use_tile_file;        // flag to use id or file
    short tile_set_id;          // id of tile_dbase to use with map
    char  tile_dbase_name[64];  // tile_dbase to use with map

    short player_spawn_count;   // # of respawns
    spawn_loc_type spawn_locs[ _MAX_SPAWN_LOCS ];

    short outpost_count;   // # of respawns
    spawn_loc_type outpost_locs[ _MAX_SPAWN_LOCS ];

    long map_data_size;
}
map_file_header_type;

typedef
struct
{
    long  obj_x_loc;      // object x_loc in world
    long  obj_y_loc;      // object y_loc in world
    short object_type;    // object classfication id;
    short sprite_type;
}
map_object_type;

typedef
struct
{
    short object_count;
}
map_object_list_header;

typedef
struct
{
    map_file_header_type     map_header;
    short                   *map_buffer;
    map_object_list_header   object_list_header;
    map_object_type         *object_list;
}
map_data_type;

#endif
