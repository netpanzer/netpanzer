/**
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

#ifndef NETPANZER_SCROLLABLETEXT_HPP
#define NETPANZER_SCROLLABLETEXT_HPP

#include "Button.hpp"
#include "Component.hpp"

class ScrollableText : public Component {
public:
    ScrollableText(std::string text, int wrapLength) : Component(), offsetY(0) {
      iXY buttonSize(20, 20);
      iXY buttonPos(size.x - buttonSize.x, 0);

      textSurface = new Surface();
      textSurface->bltStringWrapped(0, 0, text.c_str(), Color::black, componentBodyColor, wrapLength);

      upButton = new Button("upButton");
      upButton->setLabel("+");
      upButton->setLocation(buttonPos.x, buttonPos.y);
      upButton->setSize(buttonSize.x, buttonSize.y);
      upButton->setNormalBorder();
      upButton->setTextColors(Color::darkGray, Color::red, Color::gray);

      buttonPos = iXY(size.x - buttonSize.x,
                      size.y - buttonSize.y);
      downButton = new Button("downButton");
      downButton->setLabel("-");
      downButton->setLocation(buttonPos.x, buttonPos.y);
      downButton->setSize(buttonSize.x, buttonSize.y);
      downButton->setNormalBorder();
      downButton->setTextColors(Color::darkGray, Color::red, Color::gray);
    }

protected:
    int offsetY;
    Surface *textSurface;

    Button *upButton;
    Button *downButton;

public:
    virtual void actionPerformed(const mMouseEvent &me);
    void render();
};


#endif //NETPANZER_SCROLLABLETEXT_HPP
