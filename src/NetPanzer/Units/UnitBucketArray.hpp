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
#ifndef _UNIT_BUCKET_ARRAY_HPP
#define _UNIT_BUCKET_ARRAY_HPP

#include "ArrayUtil/BucketArrayTemplate.hpp"

#include "Units/UnitBase.hpp"

class UnitBucketPointer
{
public:
    UnitBase *unit;

    UnitBucketPointer()
    {
        unit = 0;
        next = 0;
        prev = 0;
    }

    UnitBucketPointer(UnitBase *unit)
    {
        UnitBucketPointer::unit = unit;
        next = 0;
        prev = 0;
    }

    UnitBucketPointer *next;
    UnitBucketPointer *prev;
};

typedef BucketArrayTemplate< UnitBucketPointer > UnitBucketArrayTemplate;
typedef LinkListDoubleTemplate< UnitBucketPointer > UnitBucketList;

class UnitBucketArray : public UnitBucketArrayTemplate
{
protected:
    long map_x_sample_factor;
    long map_y_sample_factor;
    long pixel_x_sample_factor;
    long pixel_y_sample_factor;
    iXY map_size;
    long map_size_x;
    long map_size_y;
    iXY tile_size;

public:

    UnitBucketArray( );
    ~UnitBucketArray( );

    void initialize( iXY map_size, iXY tile_size);
    void initialize( iXY map_size, iXY tile_size, long x_super_sample, long y_super_sample );

    inline long getXSample( void )
    {
        return( map_x_sample_factor );
    }

    inline long getYSample( void )
    {
        return( map_y_sample_factor );
    }

    inline iXY getSample( void )
    {
        return( iXY( map_x_sample_factor, map_y_sample_factor) );
    }

    inline unsigned long mapLocToBucketIndex( iXY map_loc )
    {
        long bucket_index;

        bucket_index = ((map_loc.y / map_y_sample_factor) * column_size) +
                       (map_loc.x / map_x_sample_factor);

        return( bucket_index );
    }

    inline unsigned long worldLocToBucketIndex( iXY world_loc )
    {
        long bucket_index;

        bucket_index = ((world_loc.y / pixel_y_sample_factor) * column_size) +
                       (world_loc.x / pixel_x_sample_factor);

        return( bucket_index );
    }

    inline iXY mapLocToBucketLoc( iXY map_loc )
    {
        return( iXY( (map_loc.x / map_x_sample_factor), (map_loc.y / map_y_sample_factor) ) );
    }

    inline iXY worldLocToBucketLoc( iXY world_loc )
    {
        return( iXY(  (world_loc.x / pixel_x_sample_factor) , (world_loc.y / pixel_y_sample_factor) ) );
    }

    inline iRect worldRectToBucketRect( iRect &world_rect )
    {
        return( iRect( world_rect.min.x / pixel_x_sample_factor,
                       world_rect.min.y / pixel_y_sample_factor,
                       world_rect.max.x / pixel_x_sample_factor,
                       world_rect.max.y / pixel_y_sample_factor  )
              );
    }

    iRect worldRectToBucketRectClip( iRect &world_rect );


    inline iRect mapRectToBucketRect( iRect &map_rect )
    {
        return( iRect( map_rect.min.x / map_x_sample_factor,
                       map_rect.min.y / map_y_sample_factor,
                       map_rect.max.x / map_x_sample_factor,
                       map_rect.max.y / map_y_sample_factor  )
              );
    }

    UnitBucketList * getBucketAssocWorldLoc( iXY world_loc );
    UnitBucketList * getBucketAssocMapLoc( iXY map_loc );

    void addUnit( UnitBase *unit );
    void addUnit( UnitBucketPointer *unit_bucket_ptr );

    long getUnitBucketIndex( UnitID unit_id );

    UnitBase * getUnit( UnitID unit_id, unsigned long bucket_index );
    UnitBase * getUnitAtWorldLoc( UnitID unit_id, iXY world_loc );
    UnitBase * getUnitAtMapLoc( UnitID unit_id, iXY map_loc );

    bool moveUnit( UnitID unit_id, unsigned long from_bucket_index, unsigned long to_bucket_index );

    bool deleteUnitBucketPointer( UnitID unit_id, iXY world_loc );
};

#endif // ** _UNIT_BUCKET_ARRAY_HPP

