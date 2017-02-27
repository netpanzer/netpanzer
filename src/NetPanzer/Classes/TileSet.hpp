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
#ifndef _TILESET_HPP
#define _TILESET_HPP

#include "Structs/TileSetStruct.hpp"
#include "Classes/WadMapTable.hpp"

namespace filesystem {
class ReadFile;
}

class TileSet
{
protected:
    bool tile_set_loaded;
    TILE_DBASE_HEADER tile_set_info;
    TILE_HEADER *tile_info;
    unsigned char *tile_data;

    unsigned long tile_size;
    unsigned long tile_count;
    void computeTileConsts( void );

    filesystem::ReadFile* partition_load_fhandle;
    unsigned long partition_load_partition_count;
    unsigned long partition_load_tile_index;
    unsigned long partition_load_mapped_index;

public:
    TileSet();
    ~TileSet();

    void readTileDbHeader(filesystem::ReadFile& file, TILE_DBASE_HEADER *header);
    void loadTileSetInfo( const char *file_path );
    void loadTileSetInfo( const char *file_path, WadMapTable &mapping_table );
    void loadTileSet( const char *file_path, WadMapTable &mapping_table );

    bool startPartitionTileSetLoad( const char *file_path, WadMapTable &mapping_table, unsigned long partitions );
    bool partitionTileSetLoad( WadMapTable &mapping_table, int *percent_complete );

    void loadTileSet( const char *file_path );
    inline unsigned char * getTile(unsigned long index) const
    {
        return( tile_data + (index * tile_size) );
    }
    inline unsigned short getTileXsize() const
    {
        return ( tile_set_info.x_pix );
    }
    inline unsigned short getTileYsize() const
    {
        return ( tile_set_info.y_pix );
    }
    inline unsigned short getTileCount() const
    {
        return ( tile_set_info.tile_count );
    }
    inline unsigned char getAverageTileColor(unsigned long index) const
    {

        return( tile_info[ index ].avg_color );
    }

    inline char getTileMovementValue( unsigned long index )
    {
        return( tile_info[ index ].move_value );
    }

    inline unsigned char getTilePixel( unsigned long index , unsigned int pixX,
                                       unsigned int pixY)
    {
        if( index < tile_count ) {
            return( *(getTile(index) + (pixY * getTileXsize()) + pixX));
        }

        return( 0 );
    }

    /*
       inline unsigned char getTilePixel( long *index , unsigned int pixX, unsigned int pixY)
        {
         if( ( (*index) >= 0) && ( (*index) < (long) tile_count) )
          { return( *(getTile( (unsigned long) index) + (pixY * getTileXsize()) + pixX)); }

         (*index) = -1;
         return( 0 );
        }
    */
};

#endif // ** _TILESET_HPP
