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

 * Created on September 20, 2008, 10:46 AM
 */

#ifndef _MINIMAP_HPP
#define	_MINIMAP_HPP

#include "Views/Components/Component.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Util/NTimer.hpp"

class MiniMap : public Component, public MapEventListener
{
public:
    MiniMap(int x, int y, int w, int h);
    virtual ~MiniMap();
    
    void draw(Surface &dest);
    
    virtual void render()
    {
        // nothing
    }
    
    void actionPerformed(const mMouseEvent &me);
    
protected:
    void onMapLoadedEvent()
    {
        LOGGER.warning("onMapLoadedEvent received........");
        regenerate();
    }
    
private:
    void regenerate();
    void drawObjectives(Surface &dest);
    void drawUnits(Surface &dest);
    void drawUnit(Surface &dest, const iXY loc, PIX color, bool forceLarge);
    void drawWorldAndMouseBox(Surface &dest);
    bool mouseinside;
    bool moving;
    iXY  mousepos;
    float xratio;
    float yratio;
    NTimer blinktimer;
    bool blinkstatus;
};

#endif	/* _MINIMAP_HPP */

