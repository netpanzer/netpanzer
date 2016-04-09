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
#ifndef _BUCKETARRAYTEMPLATE_HPP
#define _BUCKETARRAYTEMPLATE_HPP

#include <assert.h>
#include "ArrayUtil/LinkListDoubleTemplate.hpp"
#include "Util/NoCopy.hpp"

template< class TYPE >
class BucketArrayTemplate : public NoCopy
{
protected:
    size_t size;
    size_t row_size;
    size_t column_size;
    LinkListDoubleTemplate< TYPE > *array;

public:
    BucketArrayTemplate()
        : size(0), row_size(0), column_size(0), array(0)
    {
    }

    BucketArrayTemplate(size_t rows, size_t columns)
        : size(0), row_size(0), column_size(0), array(0)
    {
        initialize(rows, columns);
    }

    ~BucketArrayTemplate()
    {
        deallocate();
    }

    void initialize(size_t rows, size_t columns)
    {
        deallocate();

        row_size = rows;
        column_size = columns;

        size = row_size * column_size;

        array = new LinkListDoubleTemplate< TYPE > [ size ];
    }

    void deallocate()
    {
        delete[] array;

        array = 0;
        size = 0;
        row_size = 0;
        column_size = 0;
    }

    size_t getSize() const
    {
        return size;
    }

    void addObject(size_t bucket_index, TYPE *object)
    {
        assert( bucket_index < size );

        array[ bucket_index ].addFront( object );
    }

    void addObject(size_t row, size_t column, TYPE *object)
    {
        size_t bucket_index = (row * column_size) + column;
        assert( bucket_index < size );

        array[ bucket_index ].addFront( object );
    }

    LinkListDoubleTemplate< TYPE > * getBucket(size_t bucket_index)
    {
        assert( bucket_index < size );
        return( &(array[ bucket_index ]) );
    }

    LinkListDoubleTemplate< TYPE > * getBucket(size_t row,
            size_t column )
    {
        size_t bucket_index = (row * column_size) + column;

        assert( bucket_index < size );
        return( &(array[ bucket_index ]) );
    }
};

#endif // ** _BUCKETARRAYTEMPLATE_HPP
