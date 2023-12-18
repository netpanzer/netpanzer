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


#include "Classes/TileSet.hpp"

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Util/FileSystem.hpp"
#include "Util/Exception.hpp"

TileSet::TileSet( )
{
    tile_info = 0;
    tile_data = 0;
    tile_set_loaded = false;
    tile_count = 0;
}

TileSet::~TileSet()
{
    delete[] tile_info;
    delete[] tile_data;
}

void TileSet::readTileDbHeader(filesystem::ReadFile& file,
        TILE_DBASE_HEADER *dbHeader)
{
    file.read(&dbHeader->netp_id_header, sizeof(dbHeader->netp_id_header), 1);
    dbHeader->version = file.readULE16();
    dbHeader->x_pix = file.readULE16();
    dbHeader->y_pix = file.readULE16();
    dbHeader->tile_count = file.readULE16();
    file.read(&dbHeader->palette, sizeof(dbHeader->palette), 1);
}

void TileSet::computeTileConsts( void )
{
    tile_size = tile_set_info.x_pix * tile_set_info.y_pix;
}

void TileSet::loadTileSetInfo( const char *file_path )
{
    std::unique_ptr<filesystem::ReadFile> file (filesystem::openRead(file_path));

    delete[] tile_data;
    tile_data = 0;
    delete[] tile_info;
    tile_info = 0;
    tile_set_loaded = false;

    readTileDbHeader(*file, &tile_set_info);

    tile_count = tile_set_info.tile_count;

    tile_set_loaded = true;

    computeTileConsts();
}

void TileSet::loadTileSet( const char *file_path )
{
    unsigned long  tile_buffer_size;

    try {
	std::unique_ptr<filesystem::ReadFile> file(
                filesystem::openRead(file_path));

        delete[] tile_data;
        tile_data = 0;
        delete[] tile_info;
        tile_info = 0;
        tile_set_loaded = false;

	readTileDbHeader(*file, &tile_set_info);

	tile_buffer_size = (tile_set_info.x_pix * tile_set_info.y_pix) * tile_set_info.tile_count;

	tile_info =  new TILE_HEADER [ tile_set_info.tile_count ];
	assert( tile_info != 0 );

	tile_data =  new unsigned char[tile_buffer_size];
	assert( tile_data != 0 );

	file->read(tile_info, tile_set_info.tile_count, sizeof(TILE_HEADER));
	file->read( tile_data, tile_buffer_size, 1);

    	tile_set_loaded = true;
	
	computeTileConsts();
    } catch(std::exception& e) {
	throw Exception("Couldn't load tileset '%s': %s",
		file_path, e.what());
    }
}


void TileSet::loadTileSet( const char *file_path, WadMapTable &mapping_table )
{
    try {
	unsigned long  tile_buffer_size;
	unsigned long  tile_size;

	std::unique_ptr<filesystem::ReadFile> file(
                filesystem::openRead(file_path));

	delete[] tile_data;
	tile_data = 0;
	delete[] tile_info;
	tile_info = 0;
	tile_set_loaded = false;

	// ** Read Header Info **
    	readTileDbHeader(*file, &tile_set_info);

	// ** Read in Tile Info **
	tile_info = new TILE_HEADER [ mapping_table.used_tile_count ];

	unsigned short tile_count   = 0;
	unsigned long  tile_index   = 0;
	unsigned long  mapped_index = 0;
	unsigned long  unused_tile_count;
	unsigned long  used_tile_count;

    	tile_count = getTileCount();

	for ( tile_index = 0; tile_index < tile_count; tile_index++ ) {
	    if ( mapping_table[ tile_index ].is_used == true ) {
		file->read( (tile_info + mapped_index), sizeof(TILE_HEADER), 1);
		mapped_index++;
	    } else {
		file->seek(file->tell() + sizeof( TILE_HEADER ));
	    }
	}

	// ** Read in tile data **
	tile_buffer_size = (tile_set_info.x_pix * tile_set_info.y_pix) * mapping_table.used_tile_count;
	tile_size = (tile_set_info.x_pix * tile_set_info.y_pix);

	tile_data =  new unsigned char [tile_buffer_size];

	mapped_index = 0;
	tile_index = 0;

	while ( tile_index < tile_count ) {
	    used_tile_count = 0;
	    unused_tile_count = 0;

	    //** find the next used tile
	    while( (tile_index < tile_count) && (mapping_table[ tile_index ].is_used == false) ) {
		unused_tile_count++;
		tile_index++;
	    }

	    if ( tile_index < tile_count) {
		if (unused_tile_count != 0) {
		    file->seek(file->tell()+ tile_size * unused_tile_count);
		}

		while( (tile_index < tile_count) && (mapping_table[ tile_index ].is_used == true) ) {
		    used_tile_count++;
		    tile_index++;
		}
		
		if (used_tile_count != 0) {
		    file->read( (tile_data + (mapped_index*tile_size)),
			    tile_size , used_tile_count);
		    mapped_index += used_tile_count;
		}

    	    }	// ** if ( tile_index < tile_count)
	}

	tile_set_loaded = true;

	TileSet::tile_count = mapping_table.used_tile_count;
	computeTileConsts();
    } catch(std::exception& e) {
	throw Exception("Error while reading tileset '%s': %s",
		file_path, e.what());
    }
}


void TileSet::loadTileSetInfo( const char *file_path, WadMapTable &mapping_table )
{
    try {
	std::unique_ptr<filesystem::ReadFile> file(
                filesystem::openRead(file_path));

	delete[] tile_data;
	tile_data = 0;
	delete[] tile_info;
	tile_info = 0;
	tile_set_loaded = false;

	// ** Read Header Info **
	readTileDbHeader(*file, &tile_set_info);

	// ** Read in Tile Info **
	tile_info =  new TILE_HEADER [ mapping_table.used_tile_count ];

	unsigned short tile_count   = 0;
	unsigned long  tile_index   = 0;
	unsigned long  mapped_index = 0;

	tile_count = getTileCount();

	for ( tile_index = 0; tile_index < tile_count; tile_index++ ) {
	    if ( mapping_table[ tile_index ].is_used == true ) {
		file->read((tile_info + mapped_index), sizeof ( TILE_HEADER ), 1);
		mapped_index++;
	    } else {
		file->seek(file->tell()+sizeof( TILE_HEADER ));
	    }
	}

	tile_set_loaded = true;

	TileSet::tile_count = mapping_table.used_tile_count;
	computeTileConsts();
    } catch(std::exception& e) {
	throw Exception("Error while reading tileset '%s': %s", file_path,
		e.what());
    }
}

bool TileSet::startPartitionTileSetLoad( const char *file_path, WadMapTable &mapping_table, unsigned long partitions )
{
    unsigned long  tile_buffer_size;
    unsigned long  tile_size;

    partition_load_fhandle = filesystem::openRead(file_path);

    delete[] tile_data;
    tile_data = 0;
    delete[] tile_info;
    tile_info = 0;
    tile_set_loaded = false;

    // ** Read Header Info **
    readTileDbHeader(*partition_load_fhandle, &tile_set_info);

    // ** Read in Tile Info **
    tile_info =  new TILE_HEADER [ mapping_table.used_tile_count ];
    assert( tile_info != 0 );

    unsigned short tile_count   = 0;
    unsigned long  tile_index   = 0;
    unsigned long  mapped_index = 0;

    tile_count = getTileCount();

    for ( tile_index = 0; tile_index < tile_count; tile_index++ ) {
        if ( mapping_table[ tile_index ].is_used == true ) {
            partition_load_fhandle->read((tile_info + mapped_index), sizeof ( TILE_HEADER ), 1 );
            mapped_index++;
        } else {
            partition_load_fhandle->seek(partition_load_fhandle->tell()+sizeof( TILE_HEADER ));
        }
    }

    // ** Read in tile data **
    tile_buffer_size = (tile_set_info.x_pix * tile_set_info.y_pix) * mapping_table.used_tile_count;
    TileSet::tile_size = tile_size = (tile_set_info.x_pix * tile_set_info.y_pix);

    tile_data = new unsigned char [tile_buffer_size];
    assert( tile_data != 0 );

    partition_load_tile_index = 0;
    partition_load_mapped_index = 0;

    if( partitions == 0 ) {
        partition_load_partition_count = tile_count;
        int percent;
        partitionTileSetLoad( mapping_table, &percent );
        return( false );
    } else {
        partition_load_partition_count = tile_count / partitions;
        return( true );
    }
}

bool TileSet::partitionTileSetLoad( WadMapTable &mapping_table, int *percent_complete )
{
    unsigned long tile_count = getTileCount();
    unsigned long partition_index = 0;

    *percent_complete = -1;

    while( (partition_load_tile_index < tile_count) && (partition_index < partition_load_partition_count) ) {
        if ( mapping_table[ partition_load_tile_index ].is_used == true ) {
            partition_load_fhandle->read((tile_data + (partition_load_mapped_index*tile_size)), tile_size, 1);
            partition_load_mapped_index++;
        } else {
            partition_load_fhandle->seek(partition_load_fhandle->tell()+tile_size);
        }

        partition_index++;
        partition_load_tile_index++;
    }

    if ( partition_load_tile_index == tile_count ) {
        delete partition_load_fhandle;
        partition_load_fhandle = 0;
        tile_set_loaded = true;

        TileSet::tile_count = mapping_table.used_tile_count;
        computeTileConsts();

        *percent_complete = 100;

        return false;
    } else {
        float percent;
        percent = ( ( (float) partition_load_tile_index) / ( (float) tile_count ) ) * 100;
        *percent_complete = (int) percent;

        partition_index = 0;
        return true;
    }
}
