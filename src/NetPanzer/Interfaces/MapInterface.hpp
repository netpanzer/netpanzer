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
#ifndef _MAPINTERFACE_HPP
#define _MAPINTERFACE_HPP

#include <list>

#include "TileInterface.hpp"
#include "Classes/WorldMap.hpp"
#include "Classes/SpawnList.hpp"
#include "Classes/WadMapTable.hpp"

class MapEventListener
{
public:
    virtual ~MapEventListener() {};
protected:
    virtual void onMapLoadedEvent() = 0;
private:
    friend class MapInterface;
};


class MapInterface : protected TileInterface
{
private:
    typedef std::list<MapEventListener *> MapListenerList;
    static MapListenerList listenerList;


protected:
    static WorldMap main_map;
    static SpawnList spawn_list;
    static WadMapTable wad_mapping_table;
    static char map_path[256];
    static const int TILE_WIDTH = 32;
    static const int TILE_HEIGHT = 32;



protected:
    static void generateMappingTable();



public:
    static unsigned char craters_lifetime;
    static unsigned char craters_fading;
    static unsigned char units_shadow_blending;
    static unsigned char chat_color_scheme;

    static void addMapEventListener(MapEventListener *lis)
    {
        listenerList.push_back(lis);
    }

    static void removeMapEventListener(MapEventListener *lis)
    {
        listenerList.remove(lis);
    }

    static void getMapPointSize(iXY *map_size)
    {
        map_size->x = main_map.getWidth() * tile_set.getTileXsize();
        map_size->y = main_map.getHeight() * tile_set.getTileYsize();
    }

    static iXY getSize()
    {
        return iXY(main_map.getWidth(), main_map.getHeight());
    }

    static size_t getWidth()
    {
        return main_map.getWidth();
    }

    static size_t getHeight()
    {
        return main_map.getHeight();
    }

    static WorldMap::MapElementType MapValue(size_t x, size_t y)
    {
        return main_map.getValue(x, y);
    }

    static WorldMap::MapElementType MapValue(size_t offset)
    {
        return main_map.getValue(offset);
    }

    static size_t mapXYtoOffset(const iXY& map_loc)
    {
        return map_loc.y * main_map.getWidth() + map_loc.x;
    }

    static void offsetToMapXY(size_t offset, iXY& map_loc)
    {
        map_loc.y = offset/main_map.getWidth();
        map_loc.x = offset%main_map.getWidth();
    }

    static int mapXtoPointX(const int x) { return (x*TILE_WIDTH) + (TILE_WIDTH/2); }
    static int mapYtoPointY(const int y) { return (y*TILE_HEIGHT)+ (TILE_HEIGHT/2); }

    static void mapXYtoPointXY(const iXY& map_loc, iXY& loc)
    {
        loc.x = mapXtoPointX(map_loc.x);
        loc.y = mapXtoPointX(map_loc.y);
    }

    static void mapXYtoTopPointXY(const iXY &map_loc, iXY& loc)
    {
        loc.x = map_loc.x * TILE_WIDTH;
        loc.y = map_loc.y * TILE_HEIGHT;
    }

    static int pointXtoMapX(const int x) { return x/TILE_WIDTH; }
    static int pointYtoMapY(const int y) { return y/TILE_HEIGHT; }

    static void pointXYtoMapXY(const iXY& point, iXY& map_loc)
    {
        map_loc.x = pointXtoMapX(point.x);
        map_loc.y = pointYtoMapY(point.y);
    }

    static WorldMap* getMap()
    {
        return( &main_map );
    }

    static bool inside(const iXY& map_loc)
    {
        if(map_loc.x < 0 || map_loc.y < 0
                || map_loc.x >= (int) getWidth()
                || map_loc.y >= (int) getHeight())
            return false;

        return true;
    }

protected:
    static void finishMapLoad();

public:
    static bool startMapLoad(const char *file_path, const char *mapstyle_path, bool load_tiles, size_t partitions);
    static bool loadMap( int *percent_complete );

    static bool isMapLoaded()
    {
        return( main_map.isMapLoaded() );
    }

    static unsigned char getMovementValue( const iXY& map_loc );

    static unsigned char getAverageColorMapXY( const iXY& map_loc );

    static iXY getFreeSpawnPoint()
    {
        return spawn_list.getFreeSpawnPoint();
    }

    static SpawnList* getSpawnList()
    {
        return &spawn_list;
    }
};

#endif
