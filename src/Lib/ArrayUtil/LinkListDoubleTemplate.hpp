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
#ifndef _LINKLISTDOUBLETEMPLATE_HPP
#define _LINKLISTDOUBLETEMPLATE_HPP

#include "Util/NoCopy.hpp"

template< class TYPE >
class LinkListDoubleTemplate : public NoCopy
{
protected:
    TYPE *front;
    TYPE *rear;

public:
    LinkListDoubleTemplate()
            : front(0), rear(0)
    { }

    ~LinkListDoubleTemplate()
    {
        deallocate();
    }

    TYPE* getFront()
    {
        return front;
    }

    TYPE* getRear()
    {
        return rear;
    }

    void addRear(TYPE *object)
    {
        object->next = 0;
        object->prev = rear;

        if (rear == 0) {
            front = object;
            rear = object;
        } else {
            rear->next = object;
            rear = object;
        }
    }

    void addFront(TYPE *object)
    {
        object->next = front;
        object->prev = 0;

        if (front == 0) {
            front = object;
            rear = object;
        } else {
            front->prev = object;
            front = object;
        }
    }

    void insertAfter(TYPE *after, TYPE *object)
    {
        assert(after != 0);

        if(after->next != 0) {
            after->next->prev = object;
        }

        object->next = after->next;
        after->next = object;
        object->prev = after;

        if(after == rear) {
            rear = object;
        }
    }

    void insertBefore(TYPE *before, TYPE *object)
    {
        assert(before != 0);

        if(before->prev != 0) {
            before->prev->next = object;
        }

        object->prev = before->prev;
        before->prev = object;
        object->next = before;

        if(before == front) {
            front = object;
        }
    }

    void deleteObject(TYPE *object)
    {
        if( object->prev != 0 ) {
            object->prev->next = object->next;
        }

        if( object->next != 0 ) {
            object->next->prev = object->prev;
        }

        if(object == front) {
            front = object->next;
        }

        if(object == rear) {
            rear = object->prev;
        }

        delete object;
    }

    void deleteFront()
    {
        if(front == 0)
            return;

        TYPE* oldfront = front;

        if (front == rear) {
            front = 0;
            rear = 0;
        } else {
            front = front->next;
            front->prev = 0;
        }

        delete oldfront;
    }

    void deleteRear()
    {
        if(rear == 0)
            return;

        TYPE *oldrear = rear;

        if (front == rear) {
            front = 0;
            rear = 0;
        } else {
            rear = rear->prev;
            rear->next = 0;
        }

        delete oldrear;
    }

    void deleteAfter(TYPE *after)
    {
        assert( after != 0 );

        TYPE* object = after->next;

        if(object == 0)
            return;

        after->next = object->next;

        if(object->next != 0) {
            object->next->prev = after;
        }

        if(object == rear) {
            rear = after;
        }

        delete object;
    }

    void removeObject(TYPE *object)
    {
        if( object->prev != 0 ) {
            object->prev->next = object->next;
        }

        if( object->next != 0 ) {
            object->next->prev = object->prev;
        }

        if( object == front ) {
            front = object->next;
        }

        if( object == rear ) {
            rear = object->prev;
        }
    }


    TYPE* removeFront()
    {
        TYPE *delete_ptr;

        delete_ptr = front;

        if ( front != 0 ) {
            if (front == rear) {
                front = 0;
                rear = 0;
            } else {
                front = front->next;
                front->prev = 0;
            }

            return( delete_ptr );
        }

        return( 0 );
    }

    TYPE* removeRear()
    {
        TYPE *delete_ptr;

        delete_ptr = rear;

        if ( rear != 0 ) {
            if (front == rear) {
                front = 0;
                rear = 0;
            } else {
                rear = rear->prev;
                rear->next = 0;
            }

            return delete_ptr;
        }
        return 0;
    }

    TYPE* removeAfter( TYPE *after )
    {
        TYPE *delete_ptr;
        assert( after != 0 );

        delete_ptr = after->next;

        if( after->next != 0 ) {
            if( after->next->next != 0 ) {
                after->next->next->prev = after;
                after->next = after->next->next ;
            } else {
                after->next = 0;
            }

            if( delete_ptr == rear ) {
                rear = after;
            }

            return delete_ptr;
        }

        return 0;
    }

    TYPE* removeBefore( TYPE *before )
    {
        TYPE *delete_ptr;
        assert( before != 0 );

        delete_ptr = before->prev;

        if( before->prev != 0 ) {
            if( before->prev->prev != 0 ) {
                before->prev->prev->next = before;
                before->prev = before->prev->prev;
            } else {
                before->prev = 0;
            }

            if( delete_ptr == front ) {
                front = before;
            }

            return delete_ptr;
        }
        return 0;
    }


    size_t getSize() const
    {
        TYPE *traversal_ptr;
        size_t count = 0;

        for(TYPE* p = front; p != 0; p = p->next)
            count++;

        return count;
    }

    void deallocate()
    {
        TYPE *traversal_ptr;
        TYPE *delete_ptr;

        traversal_ptr = front;

        while( traversal_ptr != 0 ) {
            delete_ptr = traversal_ptr;
            traversal_ptr = traversal_ptr->next;
            delete delete_ptr;
        }

        front = 0;
        rear = 0;
    }
};

#endif // ** _LINKLISTDOUBLETEMPLATE_HPP

