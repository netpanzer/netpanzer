/* 
Copyright (C) 2008 by Aaron Perez <aaronps@gmail.com>

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
 *
 * Created on September 11, 2008, 7:05 PM
 */

#include <vector>
#include <algorithm>

#include "ResourceManager.hpp"
#include "2D/Surface.hpp"
#include "Util/FileSystem.hpp"

#define DEFAULT_FLAGS_PATH "pics/flags/"
#define MAX_FLAGS 256

class _RMan
{
public:
    _RMan()
    {
        for (int n = 0; n < MAX_FLAGS; n++)
        {
            flagList[n]=new Surface(20,14,1);
            flagList[n]->fill(0);
            flagUsedCount[n] = 0;
        }
    }

    ~_RMan()
    {
        if ( flagList )
        {
            for (int n = 0; n < MAX_FLAGS; n++)
            {
                delete flagList[n];
                flagList[n] = 0;
                flagUsedCount[n] = 0;
            }
        }
    }
    
    Surface * flagList[MAX_FLAGS];
    unsigned char flagUsedCount[MAX_FLAGS];
};

_RMan *RMan = 0;

void
ResourceManager::initialize()
{
    if ( ! RMan )
    {
        RMan = new _RMan();
    }
}

void
ResourceManager::finalize()
{
    if ( RMan )
    {
        delete RMan;
        RMan = 0;
    }
}

// actually loads all the flags.
int
ResourceManager::loadAllFlags(Surface& flags, std::vector<std::string>& names)
{
    char** list = filesystem::enumerateFiles(DEFAULT_FLAGS_PATH);
    std::string flagname;
    
    for(char** file = list; *file != 0; file++)
    {
        flagname = *file;
        if ( flagname.find(".bmp") != std::string::npos )
        {
            names.push_back(flagname);
        }
    }

    filesystem::freeList(list);

    flags.create( FLAG_WIDTH, FLAG_HEIGHT, names.size());
    Surface tmpflags;
    tmpflags.create( FLAG_WIDTH, FLAG_HEIGHT, 1);

    sort(names.begin(), names.end());
    std::string path(DEFAULT_FLAGS_PATH);

    for (std::vector<std::string>::size_type i = 0; i < names.size(); i++)
    {
        flags.setFrame(i);
        tmpflags.loadBMP((path+names[i]).c_str(), false);
        tmpflags.blt(flags, 0, 0);
    }
    return names.size();
}

bool
ResourceManager::loadFlag(Surface* dest, std::string name)
{
    dest->loadBMP((DEFAULT_FLAGS_PATH + name).c_str());
    return true;
}

Surface *
ResourceManager::getFlag(const FlagID flag)
{
    return RMan->flagList[flag];
}

void
ResourceManager::getFlagData(const FlagID flag, Uint8 * dest, const size_t dest_len)
{
    RMan->flagList[flag]->frameToBuffer(dest, dest_len);
}

void
ResourceManager::updateFlagData(const FlagID flag, const Uint8 * src, const size_t src_len)
{
    RMan->flagUsedCount[flag] += 1;
    RMan->flagList[flag]->bufferToFrame(src, src_len);
}

int
ResourceManager::getFlagUsedCount(const FlagID flag)
{
    return RMan->flagUsedCount[flag];
}

