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
 * Created on September 11, 2008, 6:55 PM
 */

#ifndef _RESOURCEMANAGER_HPP
#define	_RESOURCEMANAGER_HPP

#include "Core/CoreTypes.hpp"
#include <vector>
#include <string>

// forward declarations
class Surface;

class ResourceManager
{
public:
    static void initialize();
    static void finalize();

    static int loadAllFlags(Surface& flags, std::vector<std::string>& names);
    static bool loadFlag(Surface* dest, std::string name);
    static Surface * getFlag(FlagID flag);

    static void getFlagData(const FlagID flag, Uint8 * dest, const size_t dest_len);
    static void updateFlagData(const FlagID flag, const Uint8 * src, const size_t src_len);
    static int getFlagUsedCount(const FlagID flag);
};

#endif	/* _RESOURCEMANAGER_HPP */

