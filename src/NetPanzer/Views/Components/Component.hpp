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
#ifndef __Component_hpp__
#define __Component_hpp__

#include <string>

#include "2D/Color.hpp"
#include "2D/Surface.hpp"
#include "Types/iXY.hpp"
#include "Types/iRect.hpp"
#include "Util/NoCopy.hpp"
#include "Util/Log.hpp"

class mMouseEvent;

//--------------------------------------------------------------------------
class Component : public NoCopy
{
protected:
    PIX    background;
    PIX    foreground;
    iXY     size;
    iXY    position;
    Surface surface;
    std::string componentName;
    bool    enabled;
    bool    visible;
    bool    dirty;

    static int borderSize;

    void reset();

public:
    Component *next;
    Component *prev;

    void *parent;       // Who is my daddy?

    void setParent(void *parent);

    enum
    {
        BOTTOM_ALIGNMENT,
        CENTER_ALIGNMENT,
        LEFT_ALIGNMENT,
        RIGHT_ALIGNMENT,
        TOP_ALIGNMENT
    };

    Component()
    {
        reset();
    }
    
    Component(const std::string &cname) : componentName(cname)
    {
        reset();
    }
    
    virtual ~Component()
    {
    }

    // Accessor functions.
    void getBounds(iRect &r)
    {
        r.min = position;
        r.max = position + size;
    }
    bool contains(int x, int y) const;
    bool contains(iXY p) const { return contains(p.x, p.y); }
//    Uint8 getBackground() const { return background; }
//    Uint8 getForeground() const { return foreground; }
//    const std::string& getName() const { return name; }
//    const iXY &getSize() const { return size; }
//    int getSizeX() const { return size.x; }
//    int getSizeY() const { return size.y; }
//    bool isEnabled() const { return enabled; }
//    bool isVisible() const { return visible; }

    void setBounds(const iRect &r)
    {
        position  = r.min;
        size = r.getSize();
        //surface.create(r.getSizeX(), r.getSizeY(), 1);
        dirty = true;
    }
    
    virtual void setSize(int x, int y)
    {
        size.x=x;
        size.y=y;
        surface.create(x,y,1);
        dirty=true;
    }

    virtual iXY getSize()
    {
        return size;
    }

//    void setEnabled(bool _enabled) { enabled = _enabled; }
//    void setForeground(PIX _foreground) { foreground = _foreground; }
    void setLocation(int x, int y);
    void setLocation(const iXY &p) { setLocation(p.x, p.y); }
    void setName(const std::string& name) { Component::componentName = name; }
    std::string getName() { return componentName; }
    
    virtual void draw(Surface &dest)
    {
        iRect bounds;
        getBounds(bounds);
        
        if ( dirty )
            render();
        
        surface.bltTrans(dest, bounds.min.x, bounds.min.y);
    }
    
    virtual void render() = 0;
    virtual void actionPerformed(const mMouseEvent &me) = 0;
}; // end Component

#endif // end __Component_hpp__

