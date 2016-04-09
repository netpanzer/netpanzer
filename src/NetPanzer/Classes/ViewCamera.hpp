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
#ifndef _VIEWCAMERA_HPP
#define _VIEWCAMERA_HPP

#include "Interfaces/MapInterface.hpp"
#include "Interfaces/TileInterface.hpp"

class ViewCamera : private MapInterface
{
protected:
    iXY loc;
    iXY view_size;

public:
    ViewCamera();
    void scrollPlusX( long scroll_increment );
    void scrollMinusX( long scroll_increment );
    void scrollMinusY( long scroll_increment );
    void scrollPlusY( long scroll_increment );

    inline void setCamera( const iXY &new_loc )
    {
        loc.x = new_loc.x - (new_loc.x % 4);
        loc.y = new_loc.y - (new_loc.y % 4);
    }

    inline void setCameraSize( const iXY &size )
    {
        view_size = size;
    }

    void getViewStart( unsigned long view_size_x, unsigned long view_size_y,
                       int *view_start_x, int *view_start_y );

    inline void getViewWindow( iRect *view_win )
    {
        int view_start_x, view_start_y;
        getViewStart( view_size.x, view_size.y, &view_start_x, &view_start_y );
        view_win->min.x = view_start_x;
        view_win->min.y = view_start_y;
        view_win->max.x = view_start_x + view_size.x;
        view_win->max.y = view_start_y + view_size.y;
    }

    inline long getCameraDistance( const iXY &world_loc )
    {
        return( (long) (loc - world_loc).mag2() );
    }
};

#endif // ** _VIEWCAMERA_HPP
