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


#include <stdio.h>
#include <string.h>
#include <memory>

#include "MapsManager.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Util/FileSystem.hpp"

/// removes leading and trailing whitespaces from a string
static inline std::string trim(const std::string& str)
{
    using std::string;
    
    string::size_type start = str.find_first_not_of(" \t\n\r");
    string::size_type end = str.find_last_not_of(" \t\n\r");

    return string(str, start, end-start+1);
}

std::string MapsManager::getNextMap(const std::string& map)
{
    using std::string;

    // parse the mapcycle in the config
    const string& mapcycle = *GameConfig::game_mapcycle;
    string::size_type i = mapcycle.find(',', 0);
    string::size_type lasti = 0;
    if(i == string::npos)
        i = mapcycle.size();
    string currentMap = trim(string(mapcycle, 0, i));
    string firstmap = currentMap;

    bool takeNext = false;
    do {
        if(takeNext) {
            return currentMap;
        }
        if(currentMap == map)
            takeNext = true;
        
        if(i == mapcycle.size())
            break;
        lasti = i;
        i = mapcycle.find(',', i+1);
        if(i == string::npos) {
            i = mapcycle.size();
        }
        currentMap = trim(string(mapcycle, lasti+1, i-lasti-1));
    } while(1);

    return firstmap;
}

bool MapsManager::existsMap(const std::string& name)
{
    std::string basefilename = "maps/";
    basefilename += name;

    std::string filename = basefilename + ".npm";
    if(!filesystem::exists(filename))
        return false;
    filename = basefilename + ".opt";
    if(!filesystem::exists(filename))
        return false;
    filename = basefilename + ".spn";
    if(!filesystem::exists(filename))
        return false;

    return true;
}

