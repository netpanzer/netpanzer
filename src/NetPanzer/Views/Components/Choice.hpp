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
#ifndef __Choice_hpp__
#define __Choice_hpp__

#include <vector>
#include <string>

#include "Component.hpp"
#include "MouseEvent.hpp"

class StateChangedCallback;

//--------------------------------------------------------------------------
class Choice : public Component
{
private:
    std::vector<std::string> choiceList;
    size_t              index;
    int                 minWidth;
    bool                isOpen;
    size_t              mouseover;
    int                 adjustedY;  // The y translation value to keep the choice on the screen.
    StateChangedCallback* callback;
    int                 choiceItemHeight;

public:
    Choice(StateChangedCallback* newcallback = 0)
            : callback(newcallback)
    {
        reset();
    }
    virtual ~Choice()
    {}

    void   add(const std::string& item);
    void   addItem(const std::string& item);
    void   copyItems(const Choice &choice);
    size_t getItemCount() const
    {
        return choiceList.size();
    }
    size_t getSelectedIndex() const
    {
        return index;
    }
    void   select(const std::string& item);
    void   select(size_t index);
    void   setMinWidth(int minWidth);

    void setStateChangedCallback(StateChangedCallback* newcallback)
    {
        callback = newcallback;
    }

    virtual void draw(Surface &dest);
    virtual void render()
    {
    }
    virtual void actionPerformed(const mMouseEvent &me);

private:
    void reset();
}; // end Choice

#endif // end __Choice_hpp__
