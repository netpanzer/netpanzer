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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory>

#include "Classes/WorldMap.hpp"
#include "Port/MapData.hpp"
#include "Util/FileSystem.hpp"
#include "Util/Exception.hpp"
#include "Util/Endian.hpp"

WorldMap::WorldMap()
    : map_loaded(false), map_buffer(0)
{
}

WorldMap::~WorldMap()
{
    delete[] map_buffer;
}

void WorldMap::reMap( WadMapTable &mapping_table )
{
    for(size_t i = 0; i < getSize(); ++i) {
        map_buffer[i] = mapping_table[map_buffer[i]].remap_index;
    }
}

void WorldMap::loadMapFile(const std::string& filename)
{
    try {
	std::unique_ptr<filesystem::ReadFile> file(
                filesystem::openRead(filename));

	if ( map_loaded == true ) {
	    delete[] map_buffer;
	    map_buffer = 0;
	    map_loaded = false;
	}

	map_info.load(*file);

	size_t map_size = map_info.width * map_info.height;

	map_buffer = new MapElementType [ map_size ];

        for(size_t i = 0; i < map_size; ++i) {
            map_buffer[i] = file->readULE16();
        }
	
	map_loaded = true;
    } catch(std::exception& e) {
	throw Exception("Error while reading mapfile '%s': %s",
		filename.c_str(), e.what());
    }
}
