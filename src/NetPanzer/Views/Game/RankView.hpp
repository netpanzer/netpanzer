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


#ifndef __RankView_hpp__
#define __RankView_hpp__

#include <vector>
#include "Views/Components/View.hpp"
#include "2D/Surface.hpp"
#include "GameTemplateView.hpp"
#include "Views/Components/ScrollBar.hpp"

class PlayerState;

//---------------------------------------------------------------------------
class RankView : public GameTemplateView
{
private:
    int viewableMessageCount;
    enum { ITEM_GAP_SPACE = 10 };
    void drawPlayerStats(Surface &dest, unsigned int flagHeight);

    ScrollBar *scrollBar;
    Surface allyImage;
    Surface allyRequestImage;
    Surface allyOtherImage;
    Surface noAllyImage;
    Surface colorImage;
    Surface muteImageW;
    Surface muteImageR;
    Surface botImage;
    Surface humanImage;
    Surface unknownImage;
    std::vector<const PlayerState*> states;
    int selected_line;
    PlayerState* mstate;
    PlayerState* tstate;

public:

    RankView();
    virtual ~RankView()
    {
        delete scrollBar;
    }

    virtual void doDraw(Surface &windowArea, Surface &clientArea);
    virtual void notifyMoveTo();



protected:

    virtual void lMouseDown(const iXY &pos);
    virtual void mouseMove(const iXY & prevPos, const iXY &newPos);
    virtual void doActivate();
    virtual void doDeactivate();
    virtual void processEvents();
}
; // end _WIN

#endif // end __RankView_hpp__
