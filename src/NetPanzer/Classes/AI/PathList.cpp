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
#include "Classes/AI/PathList.hpp"

PathList::PathList(size_t list_size )
{
    size = list_size;
    first = 0;
    last = first;

    list = (unsigned long *) malloc( sizeof(unsigned long) * list_size );
    assert( list != 0 );
}

PathList::PathList(const PathList& other)
{
    initialize(other.size);
    *this = other;
}

PathList::~PathList()
{
    free(list);
}

void PathList::initialize(size_t list_size )
{
    size = list_size;
    first = last = 0;

    list = (unsigned long *) malloc( sizeof(unsigned long) * list_size );
    assert( list != 0 );
}

