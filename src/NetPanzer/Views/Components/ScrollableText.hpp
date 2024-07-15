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
#include "View.hpp"

class ScrollableText : public Component {
public:
    ScrollableText(View* view, std::string input_text, iRect input_rect, const PIX input_color, const PIX input_blend_color) : Component(), offsetY(0) {
      position.zero();
      text = input_text;
      rect = input_rect;
      color = input_color;
      blend_color = input_blend_color;
      offsetY = 0;
      iXY buttonSize(20, 20);
      iXY buttonPos(input_rect.getSize().x - buttonSize.x, 0);

      upButton = new Button("upButton");
      upButton->setLabel("+");
      upButton->setLocation(buttonPos.x, buttonPos.y);
      upButton->setSize(buttonSize.x, buttonSize.y);
      upButton->setNormalBorder();
      upButton->setTextColors(Color::darkGray, Color::red, Color::gray);
      view->add(upButton); // TODO support component hierarchy in View.cpp mouseMove()

      buttonPos = iXY(input_rect.getSize().x - buttonSize.x,
                      input_rect.getSize().y - buttonSize.y);
      downButton = new Button("downButton");
      downButton->setLabel("-");
      downButton->setLocation(buttonPos.x, buttonPos.y);
      downButton->setSize(buttonSize.x, buttonSize.y);
      downButton->setNormalBorder();
      downButton->setTextColors(Color::darkGray, Color::red, Color::gray);
      view->add(downButton); // TODO support component hierarchy in View.cpp mouseMove()
    }

protected:
    int offsetY;
    std::string text;
    iRect rect;
    PIX color;
    PIX blend_color;

    Button *upButton;
    Button *downButton;

public:
    virtual void actionPerformed(const mMouseEvent &me);

    void draw(Surface &dest);

    virtual void render() {}
};


#endif //NETPANZER_SCROLLABLETEXT_HPP
