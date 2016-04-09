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
#ifndef _UNITMESSAGE_HPP
#define _UNITMESSAGE_HPP

#include "Units/UnitBase.hpp"

enum { _umesg_flag_unique          = 0x01,
       _umesg_flag_broadcast       = 0x02,
       _umesg_flag_player          = 0x04,
       _umesg_flag_notplayer       = 0x08,
       _umesg_flag_send            = 0x0F,
       _umesg_flag_queue           = 0x20,
       _umesg_flag_manager_request = 0x40
     };

#ifdef MSVC
#pragma pack(1)
#endif

class UnitMessage
{
private:
    Uint16 unit_id;
public:
    Uint8 message_id;
    Uint8 message_flags;

public:
    UnitMessage()
    {
        this->unit_id = 0xBADB;
        message_id = 0;
        message_flags = 0;
    }

    UnitMessage(UnitID unit_Id, unsigned char flags)
    {
        setHeader(unit_Id, flags);
    }

    UnitID getUnitID() const
    {
        return ltoh16(unit_id);
    }

    void setHeader(UnitID unit_id, unsigned char flags )
    {
        this->unit_id = htol16(unit_id);
        message_flags = flags;
    }

    bool isFlagged(unsigned char flags) const
    {
        if ( (flags & message_flags) == flags )
            return true;

        return false;
    }
} __attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

#endif // ** _UNITMESSAGE_HPP
