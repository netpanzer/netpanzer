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
#ifndef _WORLDVIEWINTERFACE_HPP
#define _WORLDVIEWINTERFACE_HPP

#include "Classes/ViewCamera.hpp"

class WorldViewInterface
{
protected:
    static ViewCamera *main_camera;
    static ViewCamera  camera_list[10];

public:
    WorldViewInterface();

    static inline ViewCamera * getMainCamera()
    {
        return main_camera;
    }
    
    static inline void scroll_right(long scroll_increment)
    {
        main_camera->scrollPlusX(scroll_increment);
    }

    static inline void scroll_left(long scroll_increment)
    {
        main_camera->scrollMinusX(scroll_increment);
    }

    static inline void scroll_up(long scroll_increment)
    {
        main_camera->scrollMinusY(scroll_increment);
    }

    static inline void scroll_down(long scroll_increment)
    {
        main_camera->scrollPlusY(scroll_increment);
    }

    static inline void setCameraPosition(const iXY &world_loc)
    {
        main_camera->setCamera(world_loc);
    }

    static inline void setCameraSize(unsigned long view_size_x, unsigned long view_size_y)
    {
        main_camera->setCameraSize( iXY( view_size_x, view_size_y ) );
    }

    static inline void getViewWindow(iRect *view_win)
    {
        main_camera->getViewWindow( view_win );
    }

    static inline void clientXYtoWorldXY(const iRect& view_win,
                                          const iXY& client, iXY *world)
    {
        world->x = view_win.min.x + client.x;
        world->y = view_win.min.y + client.y;
    }

    static inline void worldXYtoClientXY(const iRect& view_win,
                                          const iXY& world, iXY *client )
    {
        client->x = world.x - view_win.min.x;
        client->y = world.y - view_win.min.y;
    }

    static inline iRect getViewWindow()
    {
        iRect view_win;
        main_camera->getViewWindow( &view_win );
        return( view_win );
    }

    static inline iXY getViewWindowMin()
    {
        iRect view_win;
        main_camera->getViewWindow( &view_win );
        return( view_win.min );
    }

    static inline long getCameraDistance(const iXY& world_loc)
    {
        return( main_camera->getCameraDistance( world_loc ) );
    }
};

#endif // ** _WORLDVIEWINTERFACE_HPP
