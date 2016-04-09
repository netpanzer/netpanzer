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
#ifndef _QUEUETEMPLATE_HPP
#define _QUEUETEMPLATE_HPP

#include "ArrayTemplate.hpp"

template< class TYPE >
class QueueTemplate : public ArrayTemplate< TYPE >
{
public:
    size_t front;
    size_t rear;

    QueueTemplate( )
    { }

    QueueTemplate( size_t size );

    void initialize( size_t size );

    void reset()
    {
        front = 0;
        rear = 0;
    }

    bool enqueue(const TYPE& object )
    {
        this->add( object, (rear + 1) % this->size );
        rear = (rear + 1) % this->size;

        if ( front == rear )
            return( false );

        return( true );
    }

    TYPE dequeue()
    {
        assert( front != rear );

        front = ( front + 1 ) % this->size;
        return( this->array[ front ] );
    }

    void pop()
    {
        assert( front != rear );

        front = ( front + 1 ) % this->size;
    }

    TYPE getFirst()
    {
        assert( front != rear );

        return( this->array[ (( front + 1 ) % this->size) ] );
    }

    TYPE * getFirstPtr()
    {
        assert( front != rear );

        return( &(this->array[ (( front + 1 ) % this->size) ]) );
    }


    bool isEmpty() const
    {
        return front == rear;
    }

    bool isFull () const
    {
        return front == (rear + 1) % this->size;
    }

    bool isReady() const
    {
        return front != rear;
    }

    unsigned long itemCount() const
    {
        if ( front > rear )
            return ( (rear+1) + ( (this->size-1) - front ) );
        else
            return ( rear - front );
    }
};


template< class TYPE >
QueueTemplate< TYPE >::QueueTemplate( size_t size )
        : ArrayTemplate<TYPE>( size )
{
    front = 0;
    rear = 0;
}

template< class TYPE >
void QueueTemplate< TYPE >::initialize( size_t size )
{
    ArrayTemplate< TYPE >::initialize( size );
    front = 0;
    rear = 0;
}


#endif // ** _QUEUETEMPLATE_HPP
