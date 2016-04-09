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

#include "ViewCamera.hpp"

ViewCamera::ViewCamera()
{
    loc.x = 0;
    loc.y = 0;
}

void ViewCamera::scrollPlusX( long scroll_increment )
{
    iXY view;
    iXY map_size;

    view.x = loc.x + scroll_increment;
    view.x = view.x - (view.x % 4);

    getMapPointSize( &map_size );

    if ( (view.x >= 0) && (view.x <= map_size.x) ) {
        loc.x = view.x;
    }
}

void ViewCamera::scrollMinusX( long scroll_increment )
{
    iXY view;
    iXY map_size;

    view.x = loc.x - scroll_increment;
    view.x = view.x - (view.x % 4);

    getMapPointSize( &map_size );

    if ( (view.x >= 0) && (view.x <= map_size.x) ) {
        loc.x = view.x;
    }
}

void ViewCamera::scrollMinusY( long scroll_increment )
{
    iXY view;
    iXY map_size;

    view.y = loc.y - scroll_increment;
    view.y = view.y - (view.y % 4);

    getMapPointSize( &map_size );

    if ( (view.y >= 0) && (view.y <= map_size.y) ) {
        loc.y = view.y;
    }
}

void ViewCamera::scrollPlusY( long scroll_increment )
{
    iXY view;
    iXY map_size;

    view.y = loc.y + scroll_increment;
    view.y = view.y - (view.y % 4);

    getMapPointSize( &map_size );

    if ( (view.y >= 0) && (view.y <= map_size.y) ) {
        loc.y = view.y;
    }
}

void ViewCamera::getViewStart(unsigned long view_size_x,
			      unsigned long view_size_y,
                              int *view_start_x,
                              int *view_start_y)
{
    iXY map_size;
    unsigned long view_offset_x;
    unsigned long view_offset_y;
    int start_x, start_y;

    view_offset_x = view_size_x >> 1;
    view_offset_y = view_size_y >> 1;

    start_x = loc.x - view_offset_x;
    start_y = loc.y - view_offset_y;

    getMapPointSize( &map_size );

    if ( start_x < 0 )
        start_x = 0;

    if ( start_y < 0 )
        start_y = 0;

    if ( (start_x + view_size_x) > (unsigned long) map_size.x )
        start_x = (map_size.x - view_size_x) ;

    if ( (start_y + view_size_y) > (unsigned long) map_size.y )
        start_y = (map_size.y - view_size_y);

    loc.x = start_x + view_offset_x;
    loc.y = start_y + view_offset_y;

    *view_start_x = start_x;
    *view_start_y = start_y;
}

