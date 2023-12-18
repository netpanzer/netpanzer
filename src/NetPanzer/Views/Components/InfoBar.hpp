/*
Copyright (C) 2008 by Aaron Perez <aaronps@gmail.com>

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

 * Created on September 17, 2008, 8:40 PM
 */

#ifndef _INFOBAR_HPP
#define	_INFOBAR_HPP

#include "Views/Components/Component.hpp"

class InfoBar : public Component
{
public:
    InfoBar(int x, int y)
    {
        position.x = x;
        position.y = y;
    }

    void draw(Surface &dest);

    static bool bar_on;

    virtual void render()
    {
        // nothing
    }

    void actionPerformed(const mMouseEvent &me)
    {
        // nothing
    }
};


#endif	/* _INFOBAR_HPP */

