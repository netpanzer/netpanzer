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


#include "MapFile.hpp"
#include "Util/FileSystem.hpp"
#include "Util/Exception.hpp"

void MapFile::load(filesystem::ReadFile& file)
{
    file.read(&netp_id_header, sizeof(netp_id_header), 1);
    id = file.readULE16();
    file.read(&name, sizeof(name), 1);
    file.read(&description, sizeof(description), 1);
    width = file.readULE16();
    height = file.readULE16();
    file.read(&tile_set, sizeof(tile_set), 1);
    thumbnail_width = file.readULE16();
    thumbnail_height = file.readULE16();
}
