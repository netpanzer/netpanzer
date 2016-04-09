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


#include <assert.h>
#include <string.h>
#include "BitArray.hpp"


BitArray::BitArray( void )
{
    array = 0;
}

//************************************************************************

BitArray::~BitArray( void )
{
    if ( array != 0 ) {
        delete [] array;
        array = 0;
    }
}


BitArray::BitArray( unsigned long x_size, unsigned long y_size )
{
    initialize( x_size, y_size );
}

//************************************************************************
void BitArray::initialize ( unsigned long x, unsigned long y )
{
    unsigned long rawsize;
    unsigned long remain;

    rawsize = (x * y) / 8;

    remain = rawsize % 8;

    if ( remain )
        rawsize = rawsize + 1;

    if ( array != 0 ) {
        delete [] array;
        array = 0;
    }

    array = new unsigned char [ rawsize ];
    assert( array != 0 );

    x_size = x;
    y_size = y;
    size = rawsize;

    clear();
}

//************************************************************************

void BitArray::deallocate( void )
{
    if ( array != 0 ) {
        delete [] array;
        array = 0;
    }
}


//************************************************************************
void BitArray::clear( void )
{
    memset( array, 0, size );
}

//************************************************************************
void BitArray::set( void )
{
    memset( array, 0xFF, size );
}

//************************************************************************
void BitArray::setBit( unsigned long x, unsigned long y )
{
    unsigned long index;
    unsigned char shift;
    unsigned char mask = 1;

    index = ( (y * x_size ) + x );
    shift = (unsigned char) ( 7 - (index & (unsigned long) 7 ) );   // 7 - (index % 8)
    index = index >> 3;                         // index / 8
    mask = mask << shift;

    array[ index ] = array[ index ] | mask;

}

//************************************************************************
void BitArray::clearBit( unsigned long x, unsigned long y )
{
    unsigned long index;
    unsigned char shift;
    unsigned char mask = 1;

    index = ( (y * x_size ) + x );
    shift = (unsigned char) ( 7 - (index & (unsigned long) 7 ) );
    index = index >> 3;
    mask = ~(mask << shift);

    array[ index ] = array[index] & mask;
}

//************************************************************************
bool BitArray::getBit( unsigned long x, unsigned long y )
{
    unsigned long index;
    unsigned char shift;
    unsigned char mask = 1;
    unsigned char value;

    index = ( (y * x_size ) + x );
    shift = (unsigned char) ( 7 - (index & (unsigned long) 7 ) );
    index = index >> 3;
    mask = (mask << shift);

    value = array[index] & mask;

    if ( value )
        return( true );

    return( false );

}
