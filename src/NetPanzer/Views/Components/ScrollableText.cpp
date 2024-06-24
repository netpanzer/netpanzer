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
#include "ViewGlobals.hpp"

void ScrollableText::draw(Surface &dest) {
  surface.setTo(dest);
  dest.bltStringWrapped(0, offsetY * -1, text.c_str(), Color::black, componentBodyColor, rect.getSizeX());
  upButton->draw(surface);
  downButton->draw(surface);
}

void ScrollableText::actionPerformed(const mMouseEvent &me) {
  if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED) {
    if (me.getSource() == upButton) {
      if (offsetY > 0) {
        offsetY -= 10;
      }
    } else if (me.getSource() == downButton) {
      if (offsetY < rect.getSizeY()) {
        offsetY += 10;
      }
    }
  }
}
