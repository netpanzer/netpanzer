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
#ifndef _ARRAYTEMPLATE_HPP
#define _ARRAYTEMPLATE_HPP

#include <assert.h>
#include <cstddef>

template< class TYPE >
class ArrayTemplate
{
private:
    ArrayTemplate(const ArrayTemplate& );
    void operator=(const ArrayTemplate& );

protected:
    size_t size;
    TYPE *array;

public:
    ArrayTemplate()
    {
        size = 0;
        array = 0;
    }

    ArrayTemplate(size_t newsize)
        : size(newsize)
    {
        array = new TYPE[size];
    }

    ~ArrayTemplate()
    {
        delete[] array;
    }

    void initialize(size_t newsize)
    {
        deallocate();
        array = new TYPE[newsize];
        size = newsize;
    }

    TYPE &operator[](size_t index)
    {
        assert( index < size );
        return array[index];
    }

    void add(const TYPE& object, size_t index)
    {
        assert( index < size );
        array[index] = object;
    }

    size_t getSize() const
    {
        return size;
    }

    void deallocate()
    {
        delete[] array;
        array = 0;
        size = 0;
    }
};

#endif // ** _ARRAYTEMPLATE_HPP
