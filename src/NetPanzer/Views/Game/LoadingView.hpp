/*
Copyright (C) 2003 Ivo Danihelka <ivo@danihelka.net>

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
#ifndef __LOADINGVIEW_HPP__
#define __LOADINGVIEW_HPP__

#include "Core/CoreTypes.hpp"
#include "Views/Components/View.hpp"
#include "Views/Components/Button.hpp"
#include "2D/Surface.hpp"
#include <list>

#define LINELIMIT 100

class Label;

//---------------------------------------------------------------------------
class LoadingView : public View
{
public:
    LoadingView()
    {
        init();
    }

    virtual ~LoadingView() {}

    static void update(const NPString& text);
    static void append(const NPString& text);
    static void loadFinish();
    static void loadFinish_d();
    static void loadError();
    static void show();
    static void hide();

    virtual void init();
    virtual void doActivate();
    virtual void doDeactivate();

    void doDraw(Surface &viewArea, Surface &clientArea);
    void render();

    void setNeedPassword(bool need_password);
    bool doesNeedPassword() const { return need_password; }

    void onComponentClicked(Component* c);

private:
    static std::list<NPString> lines;
    static bool dirty;

    Surface backgroundSurface;
    Surface surface;
    bool need_password;
    cInputFieldString password_str;
    cInputField * password_field;
    Button * okButton;
    Label * passwordLabel;
};

#endif // end __LOADINGVIEW_HPP__
