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
#ifndef _PATHLIST_HPP
#define _PATHLIST_HPP

#include <stdlib.h>

#include <string.h>

class PathList
{
protected:
    unsigned long *list;
    int size;
    int first;
    int last;

public:
    PathList(size_t list_size = 5000);
    PathList(const PathList& other);
    ~PathList();

    void initialize(size_t list_size);

    inline void reset()
    {
        first = 0;
        last = first;
    }

    inline bool pushFirst(int tile)
    {
        int next_first;

        next_first = (first + 1) % size;

        if ( next_first == last )
            return( false );

        first = next_first;
        list[ first ] = tile;

        return( true );
    }

    inline bool popFirst( unsigned long *tile )
    {
        if ( first == last )
            return( false );

        *tile = list[ first ];

        first = (first - 1) % size;
        if ( first < 0 )
            first = size+first;

        return( true );
    }

    inline bool pushLast( unsigned long tile )
    {
        int next_last;

        next_last = (last - 1) % size;

        if ( next_last < 0 )
            next_last = size+next_last;

        if ( first == next_last )
            return( false );

        list[ next_last ] = tile;
        last = next_last;

        return( true );
    }

    inline bool popLast( unsigned long *tile )
    {
        if ( first == last )
            return( false );

        last = (last + 1) % size;

        *tile = list[ last ];

        return( true );
    }

    inline bool take(int count)
    {
        if ( (first - count) < last )
            return( false );

        first = first - count;

        return( true );
    }

    inline bool drop(int count)
    {
        if ( (last + count) > first )
            return( false );

        last = last + count;

        return( true );
    }

    inline size_t listCount() const
    {
        if ( last > first )
            return ( (first+1) + ( (size-1) - last ) );
        else
            return ( first - last );
    }

    void operator=(const PathList &rhs)
    {
        PathList::first = rhs.first;
        PathList::last  = rhs.last;

        if ( PathList::size == rhs.size ) {
            memcpy( PathList::list, rhs.list, sizeof( unsigned long ) * rhs.size );
            PathList::size  = rhs.size;
        } else {
            PathList::list = (unsigned long *) realloc( PathList::list, sizeof( unsigned long ) * rhs.size );
            memcpy( PathList::list, rhs.list, sizeof( unsigned long ) * rhs.size );
            PathList::size = rhs.size;
        }
    }
};

#endif // ** _PATHLIST_HPP
