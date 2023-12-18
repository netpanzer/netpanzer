/*
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

#ifndef USTYLESELECTIONVIEW_HPP
#define USTYLESELECTIONVIEW_HPP

#include "Core/CoreTypes.hpp"
#include "Views/Components/View.hpp"
#include "2D/Surface.hpp"

//---------------------------------------------------------------------------
class UStyleSelectionView : public View
{
private:
    iXY loc_player_flag;
    const char * text_current;
    iXY loc_text_current;
    bool loaded;

    iRect rect;

public:
    UStyleSelectionView();
    virtual ~UStyleSelectionView()
    {}

    static unsigned char rstyle_mem;

    virtual void doActivate();
    virtual void doDeactivate();
    virtual void doDraw(Surface &windowArea, Surface &clientArea);
    virtual void checkResolution(iXY oldResolution, iXY newResolution);
    virtual void processEvents();
/*
    static unsigned char getStyleMem()
    {
    return rstyle_mem;
    }

    void setStyleMem(unsigned char style_mem)
    {
    static unsigned char rstyle_mem = style_mem;
    }
*/
    void onComponentClicked(Component* c);


    void init();
};

#endif // USTYLESELECTIONVIEW_HPP
