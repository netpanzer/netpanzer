/*
Copyright (C) 2004 Hollis Blanchard
 
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
#ifndef __ENDIAN_HPP__
#define __ENDIAN_HPP__

#include <SDL2/SDL.h>

static inline Uint16 __swap16(Uint16 val)
{
    return  (((Uint16)(val) & (Uint16)0x00ffU) << 8) |
            (((Uint16)(val) & (Uint16)0xff00U) >> 8);
}

static inline Uint32 __swap32(Uint32 val)
{
    return  (((Uint32)(val) & (Uint32)0xff000000U) >> 24) |
            (((Uint32)(val) & (Uint32)0x00ff0000U) >> 8) |
            (((Uint32)(val) & (Uint32)0x0000ff00U) << 8) |
            (((Uint32)(val) & (Uint32)0x000000ffU) << 24);
}

#ifdef WORDS_BIGENDIAN

static inline Uint16 htol16(Uint16 val) { return __swap16(val); }
static inline Uint32 htol32(Uint32 val) { return __swap32(val); }

static inline Uint16 ltoh16(Uint16 val) { return __swap16(val); }
static inline Uint32 ltoh32(Uint32 val) { return __swap32(val); }

static inline Uint16 htob16(Uint16 val) { return val; }
static inline Uint32 htob32(Uint32 val) { return val; }

static inline Uint16 btoh16(Uint16 val) { return val; }
static inline Uint32 btoh32(Uint32 val) { return val; }

#else // WORDS_BIGENDIAN

/** convert a 16bit value from host to little-endian format */
static inline Uint16 htol16(Uint16 val) { return val; }
/** convert a 32bit value from host to little-endian format */
static inline Uint32 htol32(Uint32 val) { return val; }

/** convert a 16bit value from little-endian to host format */
static inline Uint16 ltoh16(Uint16 val) { return val; }
/** convert a 32bit value from little-endian to host format */
static inline Uint32 ltoh32(Uint32 val) { return val; }

/** convert a 16bit value from host to big-endian format */
static inline Uint16 htob16(Uint16 val) { return __swap16(val); }
/** convert a 32bit value from host to big-endian format */
static inline Uint32 htob32(Uint32 val) { return __swap32(val); }

/** convert a 16bit value from big-endian to host format */
static inline Uint16 btoh16(Uint16 val) { return __swap16(val); }
/** convert a 32bit value from big-endian to host format */
static inline Uint32 btoh32(Uint32 val) { return __swap32(val); }

#endif // WORDS_BIGENDIAN

#endif // __ENDIAN_HPP__
