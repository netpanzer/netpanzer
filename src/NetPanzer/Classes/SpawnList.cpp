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

#include <memory>

#include "Classes/SpawnList.hpp"
#include "Util/FileSystem.hpp"
#include "Util/FileStream.hpp"
#include "Util/Exception.hpp"
#include "Interfaces/GameConfig.hpp"

SpawnList::SpawnList()
{
    last_spawn_index = 0;
}

void SpawnList::loadSpawnFile(const std::string& file_path)
{
    clear();

    try {
        IFileStream in(file_path);

        std::string dummy;
        size_t spawn_count;
        in >> dummy >> spawn_count;
        if(!in.good())
            throw Exception("file too short");

        long x, y;
        for (size_t i = 0; i < spawn_count; ++i) {
            in >> dummy >> x >> y;
            if(!in.good())
                throw Exception("file too short");

            push_back(iXY(x, y));
        }

        last_spawn_index = 0;
    } catch(std::exception& e) {
        throw Exception("Couldn't load spawnpoints in '%s': %s",
                file_path.c_str(), e.what());
    }
}

iXY SpawnList::getFreeSpawnPoint()
{
    size_t spawn_index;

    switch ( GameConfig::game_respawntype )
    {
        case _game_config_respawn_type_round_robin :
            spawn_index = (last_spawn_index + 1) % size();
            last_spawn_index = spawn_index;
            break;
        case _game_config_respawn_type_random :
            spawn_index = rand() % size();
            break;
        case _game_config_respawn_type_random_alt :
            spawn_index = rand() % size();
            while (spawn_index == last_spawn_index) {
            spawn_index = rand() % size();
            }
            last_spawn_index = spawn_index;
            break;
        default:
            throw Exception("unknown respawn type");
    }

    return at(spawn_index);
}
