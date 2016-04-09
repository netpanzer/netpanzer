/*
Copyright (C) 2004 by Matthias Braun

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
#ifndef __NETTYPES_HPP__
#define __NETTYPES_HPP__

#include "Util/Endian.hpp"

namespace net
{

typedef Uint8 UInt8;
typedef Sint8 Int8;

class UInt32
{
private:
    Uint32 val;
public:
    UInt32(Uint32 val)
    {
        *this = val;
    }

    operator Uint32() const
    {
        return ltoh32(val);
    }
    void operator=(Uint32 val)
    {
        this->val = htol32(val);
        return val;
    }
} __attribute__((packed));

class UInt16
{
private:
    Uint16 val;
public:
    UInt32(Uint16 val)
    {
        *this = val;
    }

    operator Uint16() const
    {
        return ltoh16(val);
    }
    void operator=(Uint16 val)
    {
        this->val = htol16(val);
        return val;
    }
} __attribute__((packed));

} // end of namespace net

#endif

