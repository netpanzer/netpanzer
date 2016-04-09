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
#ifndef __UNITOPCODEDECODER_HPP__
#define __UNITOPCODEDECODER_HPP__

#include "Units/UnitOpcodes.hpp"
#include "Classes/Network/UnitNetMessage.hpp"

class UnitOpcodeDecoder
{
private:
    UnitOpcodeMessage opcode_message;
    size_t opcode_index;
    size_t opcode_size;

    void reset();

public:
    UnitOpcodeDecoder();
    ~UnitOpcodeDecoder();

    void setMessage(const NetMessage* message, size_t size);
    bool decode(UnitOpcode** opcode);
};

#endif

