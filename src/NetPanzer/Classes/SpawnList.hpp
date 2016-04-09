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
#ifndef _SPAWNLIST_HPP
#define _SPAWNLIST_HPP

#include <vector>
#include <string>
#include "Types/iXY.hpp"

class SpawnList : public std::vector<iXY>
{
private:
    size_t last_spawn_index;

public:
    SpawnList();

    void loadSpawnFile(const std::string& file_path);
    iXY getFreeSpawnPoint();
};

#endif // ** _SPAWNLIST_HPP
