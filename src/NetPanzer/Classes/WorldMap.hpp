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
#ifndef _WORLDMAP_HPP
#define _WORLDMAP_HPP

#include "Util/Endian.hpp"
#include "MapFile.hpp"
#include "Classes/WadMapTable.hpp"
#include <string>

class WorldMap
{
public:
    typedef Uint16 MapElementType;
    
    WorldMap();
    ~WorldMap();

    void loadMapFile(const std::string& filename);

    void reMap( WadMapTable &mapping_table );

    bool isMapLoaded() const
    {
        return map_loaded;
    }

    std::string getName() const
    {
        return std::string(map_info.name);
    }

    size_t getWidth() const
    {
        return map_info.width;
    }

    size_t getHeight() const
    {
        return map_info.height;
    }

    size_t getSize() const
    {
        return getWidth() * getHeight();
    }

    MapElementType getValue(size_t x, size_t y) const
    {
        assert(x < getWidth());
        assert(y < getHeight());
        return map_buffer[y*getWidth() + x];
    }

    /** @deprecatet */
    MapElementType getValue(size_t offset) const
    {
        assert(offset < getSize());
        return map_buffer[offset];
    }

    void setMapValue(size_t x, size_t y, MapElementType value)
    {
        assert(x < getWidth());
        assert(y < getHeight());
       
        map_buffer[y*getWidth() + x] = value;
    }

    const char* getAssocTileSet() const
    {
        return map_info.tile_set;
    }

private:
    bool map_loaded;               
    MapFile map_info;
    MapElementType *map_buffer;
};

#endif // ** _WORLDMAP_HPP
