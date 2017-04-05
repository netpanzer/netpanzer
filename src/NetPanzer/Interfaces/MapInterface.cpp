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
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Scripts/ScriptManager.hpp"
#include "Interfaces/GameManager.hpp"
#include "Classes/Network/NetworkState.hpp"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Util/Exception.hpp"
#include "Util/Endian.hpp"
#include "Port/MapData.hpp"
#include "Util/Log.hpp"

WorldMap MapInterface::main_map;
SpawnList MapInterface::spawn_list;
WadMapTable MapInterface::wad_mapping_table;
char MapInterface::map_path[256];
MapInterface::MapListenerList MapInterface::listenerList;

    unsigned char MapInterface::craters_lifetime;
    unsigned char MapInterface::craters_fading;
    unsigned char MapInterface::units_shadow_blending;
    unsigned char MapInterface::chat_color_scheme;

bool MapInterface::startMapLoad( const char *file_path, const char *mapstyle_mpath, bool load_tiles,
        size_t partitions )
{
    char path[256];
    char tile_set_path[256];

    strcpy( map_path, file_path );

    strcpy( path, file_path );
    strcat( path, ".npm" );
    main_map.loadMapFile( path );

    // changing the path
    NPString stpath = mapstyle_mpath;
    NPString tsname = main_map.getAssocTileSet();

    char subbuff[24];
    memcpy( subbuff, &tsname[0], 23); // max 23 chars for tileset name
    char * pch;
    pch = strstr (subbuff,".tls");
    strncpy (pch,"\0",1);

    NPString tsnamesub = subbuff;

    NPString basepath = "wads/"+tsnamesub+stpath+"/";

    // tilesets configs mgmt
    NPString cfgnamepath;
    if ( NetworkState::status == _network_state_server )
    {
    cfgnamepath = basepath+GameConfig::game_mapstyle->c_str()+".cfg";
    } else {
    cfgnamepath = basepath+GameManager::stmapstyle+".cfg";
    }
    bool okidoki = ScriptManager::loadSimpleConfig(cfgnamepath);
    if (okidoki) {

                  craters_lifetime = ScriptManager::getIntField("craters_lifetime",    0);
                  craters_fading = ScriptManager::getIntField("craters_fading",    0);
                  units_shadow_blending = ScriptManager::getIntField("units_shadow_blending",   0);
                  chat_color_scheme = ScriptManager::getIntField("chat_color_scheme",   0);

                 } else {
                 throw Exception("cannot read tileset config file.");
                 }

    //

    const char * cfulltspath = basepath.c_str();

    strcpy( tile_set_path, cfulltspath );
    strcat( tile_set_path, main_map.getAssocTileSet() );

    tile_set.loadTileSetInfo( tile_set_path );
    generateMappingTable();
    main_map.reMap( wad_mapping_table );

    if ( load_tiles == true ) {
        if ( tile_set.startPartitionTileSetLoad( tile_set_path, wad_mapping_table, partitions ) == false ) {
            finishMapLoad();
            return false;
        }

    } else {
        tile_set.loadTileSetInfo( tile_set_path, wad_mapping_table );
        finishMapLoad();
        return false;
    }

    return true;
}

bool MapInterface::loadMap( int *percent_complete )
{
    if( tile_set.partitionTileSetLoad( wad_mapping_table, percent_complete ) == false ) {
        finishMapLoad();
        return( false );
    } else {
        return( true );
    }
}


void MapInterface::finishMapLoad( void )
{
    char path[256];

    wad_mapping_table.deallocate();

    MapListenerList::iterator i = listenerList.begin();
    while ( i != listenerList.end() )
    {
        (*i)->onMapLoadedEvent();
        i++;
    }

    strcpy( path, map_path );

    strcat( path, ".spn" );
    spawn_list.loadSpawnFile( path );
}

/*
void MapInterface::LoadMap( const char *file_path, bool load_tiles )
 {
  char path[80];

  strcpy( path, file_path );
  strcat( path, ".npm" );

  tile_set.loadTileSetInfo( "./wads/wad.tls" );
  generateMappingTable();
  main_map.reMap( wad_mapping_table );

  if ( load_tiles == true )
   {
    tile_set.loadTileSet( "./wads/wad.tls", wad_mapping_table );
   }
  else
   {
    tile_set.loadTileSetInfo( "./wads/wad.tls", wad_mapping_table );
   }

  wad_mapping_table.deallocate();

  strcpy( path, file_path );
  strcat( path, ".spn" );
  spawn_list.loadSpawnFile( path );
 }
*/

void MapInterface::generateMappingTable()
{
    unsigned short tile_count;
    size_t map_size;
    size_t map_index;

    tile_count = tile_set.getTileCount();
    map_size = main_map.getSize();

    wad_mapping_table.initialize( tile_count );
    wad_mapping_table.resetMappingTable();

    // ** Find all of the used tiles **
    unsigned short tile_index;

    for ( map_index = 0; map_index < map_size; map_index++ ) {
        tile_index = main_map.getValue(map_index);
        wad_mapping_table[ tile_index ].is_used = true;
    } // ** for

    // ** Remap the used tiles **
    unsigned short mapping_index = 0;

    for ( tile_index = 0; tile_index < tile_count; tile_index++ ) {
        if ( wad_mapping_table[ tile_index ].is_used == true ) {
            wad_mapping_table[ tile_index ].remap_index = mapping_index;
            mapping_index++;
        }
    }

    wad_mapping_table.used_tile_count = mapping_index;

}

unsigned char MapInterface::getMovementValue( const iXY& map_loc )
{
    unsigned short tile_val;
    char move_val;

    if (      (map_loc.x >= 0) && ((size_t) map_loc.x < main_map.getWidth() )
              && (map_loc.y >= 0) && ((size_t) map_loc.y < main_map.getHeight() )
       ) {
        tile_val = main_map.getValue( (unsigned short) map_loc.x, (unsigned short) map_loc.y ) ;
        move_val = tile_set.getTileMovementValue( tile_val );

        switch( move_val ) {
        case 0 :
            return( 1 );

        case 1 :
            return( 1 );

        case 2 :
            return( 25 );

        case 3 :
            return( 50 );

        case 4 :
            return( 0xFF );

        case 5 :
            return( 0xFF );
        } // ** switch

        return( 1 );
    } else {
        return( 0xFF );
    }

}

unsigned char MapInterface::getAverageColorMapXY( const iXY& map_loc )
{
    unsigned short map_value;

    map_value = main_map.getValue( map_loc.x, map_loc.y );

    return( tile_set.getAverageTileColor( map_value ) );
}
