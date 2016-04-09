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
#ifndef __GetSessionView_hpp__
#define __GetSessionView_hpp__

#include "Views/Components/View.hpp"
#include "2D/Surface.hpp"
#include "Views/MainMenu/MenuTemplateView.hpp"

void bNext();


//---------------------------------------------------------------------------
class GetSessionView : public MenuTemplateView
{
protected:
    void drawHostInfo(Surface &dest, const iRect &rect);
    void drawJoinInfo(Surface &dest, const iRect &rect);
    void drawInfo(Surface &dest);

    virtual void loadBackgroundSurface();
    virtual void loadTitleSurface();

public:
    GetSessionView();

    virtual void doDraw(Surface &windowArea, Surface &clientArea);
    virtual void doActivate();
    void onComponentClicked(Component* c);
}; // end GetSessionView

#endif // end __GetSessionView_hpp__
