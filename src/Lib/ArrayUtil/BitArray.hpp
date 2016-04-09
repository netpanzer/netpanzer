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
#ifndef _BITARRAY_HPP
#define _BITARRAY_HPP

#include <stdlib.h>

#include "Util/NoCopy.hpp"

class BitArray : public NoCopy
{
private:
    unsigned char *array;

public:
    unsigned long size;
    unsigned long y_size;
    unsigned long x_size;


    BitArray( void );

    BitArray( unsigned long x_size, unsigned long y_size );

    ~BitArray( void );

    void initialize( unsigned long x_size, unsigned long y_size );

    void deallocate( void );

    void clear( void );

    void set( void );

    void setBit( unsigned long x, unsigned long y );

    void clearBit( unsigned long x, unsigned long y );

    bool getBit( unsigned long x, unsigned long y );
};



#endif // ** _BITARRAY_HPP
