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
//---------------------------------------------------------------------------

#ifndef __Desktop_hpp__
#define __Desktop_hpp__

#include <vector>
#include "Views/Components/View.hpp"
#include "Util/TimeStamp.hpp"
#include "Util/Log.hpp"

class Surface;

class Desktop
{
private: // Variables
    enum { RESIZE_NONE,
           RESIZE_TOPLEFT,
           RESIZE_TOPRIGHT,
           RESIZE_BOTTOMLEFT,
           RESIZE_BOTTOMRIGHT };

    static float      totalMouseDownTime;
    static float      currentMouseDownTime;
    static std::vector<View*> views;
    static View      *focus;
    static int        mouseActions;
    static iXY        lMouseDownPos;
    static iXY        rMouseDownPos;
    static iXY        prevMousePos;
    static int        prevButton;
    static int        mouseIsDown;
    static View      *lMouseView;
    static View      *rMouseView;
    static View      *mouseView;
    static View      *prevMouseView;
    static TimeStamp  lDoubleClickDeadline;
    static TimeStamp  rDoubleClickDeadline;
    static float      doubleClickTime;
    static int        mouseMoveStatus;

    // Movement events. MOVE THIS OUT OF THIS CLASS!  INTO GAME_WIN.
    enum { MM_LEFT  = (1U << 0) };
    enum { MM_RIGHT = (1U << 1) };
    enum { MM_UP    = (1U << 2) };
    enum { MM_DOWN  = (1U << 3) };

    // The following are used when the window is moved using move()
    static iXY mouseActionOffset; // The displacement from the mouse press to th window

private: // Functions
    static void isMouseInBox(int mouseX, int mouseY, int x1, int y1, int x2, int y2);

    static unsigned isMouseOverResizeArea(int mouseX, int mouseY);
    static bool     isMouseOverMoveArea  (int mouseX, int mouseY);

    static bool   isMouseInView(int mouseX, int mouseY);
    static View *findViewContaining(iXY p);
    static void   doMouseActions(const iXY &mousePos);

public:
    Desktop();

    static void manage(int mouseX, int mouseY, int curButton);
    static void activate(View *view);
    static void toggleVisibility(const char *viewName);
    static void setVisibility(const char *viewName, int isVisible);
    static void toggleVisibilityNoDoAnything(const char *viewName);
    static void setVisibilityNoDoAnything(const char *viewName, int isVisible);
    static void add(View *view, bool autoActivate = true);
    static void clearAll();
    static void remove(View *view);
    static void draw(Surface& surface);

    static iXY      getMouseActionOffset  ()
    {
        return mouseActionOffset;
    }
    static void     resetMouseActionOffset()
    {
        mouseActionOffset.zero();
    }

    static int         getViewCount();
    static const char *getViewTitle(int viewNum);
    static const char *getViewSearchName(int viewNum);
    static int         getViewStatus(const char *searchName);
    static void        setActiveView(const char *viewitle);
    static void        setActiveView(View *view);

    static void toggleVisibility(View *view)
    {
        toggleVisibility(view->getSearchName());
    }

    static void setVisibility(View *view, int isVisible)
    {
        setVisibility(view->getSearchName(), isVisible);
    }

    static void setVisibilityAllWindows(int isVisible)
    {
        int viewCount = getViewCount();

        for (int i = 0; i < viewCount; i++) {
            setVisibility(getViewSearchName(i), isVisible);
        }
    }

    static void toggleVisibilityNoDoAnything(View *view)
    {
        toggleVisibilityNoDoAnything(view->getSearchName());
    }

    static void setVisibilityNoDoAnything(View *view, int isVisible)
    {
        setVisibilityNoDoAnything(view->getSearchName(), isVisible);
    }

    static bool getVisible(const char *searchName)
    {
        if (getViewStatus(searchName) & View::STATUS_VISIBLE) {
            return true;
        }

        return false;
    }

    static View *getView(const char *searchName)
    {
        std::vector<View*>::iterator i;
        for(i = views.begin(); i != views.end(); i++) {
            View* view = *i;

            if (strcmp(view->searchName, searchName) == 0) {
                return view;
            }
        }

        return 0;
    }

    static void checkViewPositions(iXY viewarea);
    static void checkResolution(iXY oldResolution, iXY newResolution);

    static const View *getFocus()
    {
        return focus;
    }
    static void setFocusView(View *view)
    {
        activate(view);
    }
}
; // end Desktop

class DesktopView : public View
{
public:
    DesktopView();

    virtual void doDraw(Surface &viewArea, Surface &clientArea);
    virtual void rMouseDrag(const iXY &downPos, const iXY &prevPos, const iXY &newPos);
    virtual void doActivate();

}
; // end DesktopView


#endif // end __Desktop_hpp__
