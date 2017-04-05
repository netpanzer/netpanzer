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

#ifndef __CreditsScrollView_hpp__
#define __CreditsScrollView_hpp__

#include <vector>
#include <string>

#include "Views/MainMenu/SpecialButtonView.hpp"
#include "2D/Surface.hpp"
#include "Views/Components/ScrollBar.hpp"
#include "Views/Components/Button.hpp"

//---------------------------------------------------------------------------
class CreditsScrollView : public SpecialButtonView
{
protected:
    int columns;
    int rows;

    int  maxYOffset;
    void drawHelpText(Surface &dest, const int &x, const int &y);

    ScrollBar *scrollBar;
    std::vector<std::string> text;

    enum { TEXT_GAP_SPACE = 3 };
    void insert(const char *string);

    Button * upButton;
    Button * downButton;

    int maxViewableItems;
    int topViewableItem;

public:
    CreditsScrollView();
    virtual ~CreditsScrollView()
    {
        delete scrollBar;
    }

    virtual void doDraw(Surface &windowArea, Surface &clientArea);
    virtual void actionPerformed(mMouseEvent me);
    virtual void doActivate();
    //virtual void processEvents();

}
;
#endif


