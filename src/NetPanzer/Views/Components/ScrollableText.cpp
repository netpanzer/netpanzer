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

#include "ScrollableText.hpp"
#include "MouseEvent.hpp"

void ScrollableText::draw(Surface &dest) {
  surface.setTo(dest);
  maxY = dest.bltStringWrapped(0, offsetY * -1, text.c_str(), color, blend_color, rect.getSizeX());
  upButton->draw(surface);
  downButton->draw(surface);
  scroll();
}

void ScrollableText::setScrolling(bool isScrolling, int8_t newDirection) {
  if (!isScrolling) {
    scrollStartTime = 0;
    return;
  }
  scrollStartTime = SDL_GetTicks64();
  direction = newDirection;
};

void ScrollableText::scroll() {
  if (!scrollStartTime) {
    return;
  }
  int amount = 20;
  const uint64_t delta = SDL_GetTicks64() - scrollStartTime;
  if (delta > 200) {
    amount = 35;
  }
  if (direction < 0) {
    amount = amount * direction;
  }
  if (direction > 0) {
    if (offsetY < maxY) {
      offsetY += amount;
    }
  } else {
    if (offsetY > 0) {
      offsetY += amount;
    }
  }
}

void ScrollableText::actionPerformed(const mMouseEvent &me) {
  if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED) {
    if (me.getSource() == upButton) {
      setScrolling(true, -1);
    } else if (me.getSource() == downButton) {
      setScrolling(true, 1);
    } else {
      setScrolling(false, 0);
    }
  } else if (me.getID() == mMouseEvent::MOUSE_EVENT_RELEASED) {
    setScrolling(false, 0);
  }
}
