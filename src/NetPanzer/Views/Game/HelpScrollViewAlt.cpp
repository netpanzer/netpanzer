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

#include "HelpScrollViewAlt.hpp"

#include "Classes/WorldInputCmdProcessor.hpp"
#include "GameView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Views/Components/ScrollableText.hpp"
#include "Views/MainMenu/HelpScrollView.hpp"

// HelpScrollView
//---------------------------------------------------------------------------
HelpScrollViewAlt::HelpScrollViewAlt() : View() {
  setSearchName("HelpScrollViewAlt");
  setTitle("Help Information");
  setSubTitle("");

  setAllowResize(false);
  setAllowMove(false);

  moveTo(bodyTextRect.min);
  resize(bodyTextRect.getSize());

  scrollableText = new ScrollableText(this, HelpScrollView::getHelpText(), bodyTextRect);
}  // end HelpScrollView::HelpScrollView

void HelpScrollViewAlt::doDraw(Surface &viewArea, Surface &clientArea) {
  if (Desktop::getVisible("GameView")) {
    bltViewBackground(viewArea);
  }
  scrollableText->draw(clientArea);
  View::doDraw(viewArea, clientArea);
}

void HelpScrollViewAlt::doActivate() { /* empty */
}

void HelpScrollViewAlt::actionPerformed(mMouseEvent me) {
  scrollableText->actionPerformed(me);
}

void HelpScrollViewAlt::processEvents() {
  if (Desktop::getVisible("GameView")) COMMAND_PROCESSOR.process(false);
}
