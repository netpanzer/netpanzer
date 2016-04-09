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


#include <string.h>
#include "NetMessageDecoder.hpp"
#include "Util/Log.hpp"

NetMessageDecoder::NetMessageDecoder()
{
    memset(&decode_message, 0, sizeof(decode_message));
}

NetMessageDecoder::~NetMessageDecoder()
{
}

void
NetMessageDecoder::setDecodeMessage(const NetMessage* message, const size_t size)
{
    if ( size > sizeof(decode_message) )
    {
        LOGGER.warning("Multimessage with wrong size!");
        memset(&decode_message, 0, sizeof(decode_message));
        return;
    }

    memcpy(&decode_message, message, size);
    this->size = size;
    offset = 0;
}

Uint16
NetMessageDecoder::decodeMessage(NetMessage **message)
{
    if ( sizeof(NetMessage) + offset >= size )
    {
        return 0; // no more messages
    }

    Uint16* mlen = (Uint16*)(decode_message.data + offset);
    unsigned int msg_len = ltoh16(*mlen);

    if( msg_len > size - sizeof(NetMessage) - offset)
    {
        LOGGER.warning("Malformed Multimessage!!");
        return false;
    }

    *message = (NetMessage *)(mlen+1);

    offset += msg_len + 2;

    return msg_len;
}

