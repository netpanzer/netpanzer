/*
Copyright (C) 2004 Hollis Blanchard <hollis@penguinppc.org>
 
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
#ifndef _MAPFILE_HPP
#define _MAPFILE_HPP

#include <SDL2/SDL.h>

namespace filesystem
{
class ReadFile;
}

class MapFile {
public:
    void load(filesystem::ReadFile& file);

    char netp_id_header[64];
    Uint16 id;
    char name[256];
    char description[1024];
    Uint16 width;
    Uint16 height;
    char tile_set[256];

    Uint16 thumbnail_width;
    Uint16 thumbnail_height;
};

#endif // ** _MAPFILE_HPP
