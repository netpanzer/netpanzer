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

#include "CreditsScrollView.hpp"

#include "Views/Components/Desktop.hpp"
#include "Views/GameViewGlobals.hpp"
#include "package.hpp"
// #include "Classes/WorldInputCmdProcessor.hpp"
//---------------------------------------------------------------------------
CreditsScrollView::CreditsScrollView() : SpecialButtonView() {
  setSearchName("CreditsScrollView");
  setTitle("Credits Information");
  setSubTitle("");

  setAllowResize(false);
  setAllowMove(false);

  moveTo(bodyTextRect.min);
  resize(bodyTextRect.getSize());
  // moveTo(iXY(0, 0));
  // resize(iXY(800, 600));

  char credits[] =
      "\
  This application is free software under the terms of the\n\
  Gnu General Public license (GPL). See the COPYING file for details.\n\
\n\
  Source and binaries: https://github.com/netpanzer/netpanzer/\n\
\n\
  Website: https://netpanzer.io\n\
  - for binaries, maps, flags, related tools and news\n\
  - and don't forget to register your nickname for the ranking\n\
\n\
  Current Development Team\n\
  - Devon Winrick (winrid)\n\
  - Fulvio Testi (fu)\n\
  - Andy Alt\n\
\n\
\n\
  Authors\n\
\n\
  Additional graphics:\n\
  Peter Lisker (Nessie), Jesus Eugenio (Silvestre)\n\
\n\
\n\
  Original Game (Pyrosoft):\n\
  Vlad Rahkoy, Skip Rhudy, Matt Bogue, Clint Bogue\n\
\n\
  Linux Port, Polishing, Packaging and Related Tools:\n\
\n\
  Matthias Braun, Ivo Danihelka, Hollis Blanchard, Hankin Chick, \n\
  BenUrban Tyler Nielsen, Bastosz Fenski, Tobias Blerch, Ingo Ruhnke\n\
\n\
  0.8.3 Release:\n\
  Aaron Perez (krom), C-D, fu\n\
\n\
  0.8.4 Release up to 0.8.5-test-1:\n\
  Aaron Perez (krom), Laurant Jacques (Wile64), C-D, fu\n\
\n\
  0.8.7 Release:\n\
  Fulvio Testi (fu), Guido Ueffing (Lohengrin)";

  char *sStart = credits;
  char *sEnd = strchr(sStart, '\n');
  while (sEnd != NULL) {
    *sEnd = '\0';
    insert(sStart);
    sStart = sEnd + 1;
    sEnd = strchr(sStart, '\n');
  }
  if (*sStart != '\0') insert(sStart);

  // insert("  Alt + '-'                        Decrease brightness");
  // insert("  Alt + '='                        Increase brightness");

  int CHAR_YPIX = Surface::getFontHeight();
  maxViewableItems =
      (getClientRect().getSizeY() - (TEXT_GAP_SPACE + CHAR_YPIX)) /
          (TEXT_GAP_SPACE + CHAR_YPIX) -
      1;
  topViewableItem = 0;

  iXY size(20, 20);
  iXY pos(getClientRect().getSizeX() - size.x, 0);

  upButton = new Button("upButton");
  upButton->setLabel("+");
  upButton->setLocation(pos.x, pos.y);
  upButton->setSize(size.x, size.y);
  upButton->setNormalBorder();
  upButton->setTextColors(Color::darkGray, Color::red, Color::gray);
  add(upButton);

  pos = iXY(getClientRect().getSizeX() - size.x,
            getClientRect().getSizeY() - size.y);
  downButton = new Button("downButton");
  downButton->setLabel("-");
  downButton->setLocation(pos.x, pos.y);
  downButton->setSize(size.x, size.y);
  downButton->setNormalBorder();
  downButton->setTextColors(Color::darkGray, Color::red, Color::gray);
  add(downButton);
}

// doDraw
//---------------------------------------------------------------------------

void CreditsScrollView::doDraw(Surface &viewArea, Surface &clientArea) {
  // if (Desktop::getVisible("GameView")) {
  //     bltViewBackground(viewArea);
  // }

  // clientArea.FillRoundRect(getClientRect(), 10, Color::darkGray);
  drawHelpText(clientArea, 0, 0);

  clientArea.bltString(4, clientArea.getHeight() - Surface::getFontHeight(),
                       "Note: Use the right mouse button to scroll quickly.",
                       windowTextColor, Color::white);
  // char strBuf[256];
  // sprintf(strBuf, "%d", scrollBar->getValue());
  // clientArea.bltStringCenter(strBuf, Color::red);

  View::doDraw(viewArea, clientArea);
}

// drawHelpText
//--------------------------------------------------------------------------
void CreditsScrollView::drawHelpText(Surface &dest, const int &, const int &) {
  // PIX color   = windowTextColor;
  PIX color = Color::black;
  // if (scrollBar != 0)
  //{
  //	int minView = scrollBar->getValue();
  //	int maxView = minView + scrollBar->getViewableAmount();
  //
  //	if(maxView > scrollBar->getMaximum())
  //	{
  //		maxView = scrollBar->getMaximum();
  //	}
  //
  int curIndex = 0;
  for (int i = topViewableItem; i < topViewableItem + maxViewableItems; i++) {
    dest.bltString(1,
                   6 + curIndex * (TEXT_GAP_SPACE + Surface::getFontHeight()),
                   text[i].c_str(), color, Color::white);
    curIndex++;
  }
  //}
}

// insert
//--------------------------------------------------------------------------
void CreditsScrollView::insert(const char *string) {
  text.push_back(std::string(string));
}

// actionPerformed
//--------------------------------------------------------------------------
void CreditsScrollView::actionPerformed(mMouseEvent me) {
  if ((me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED) ||
      (me.getID() == mMouseEvent::MOUSE_EVENT_RELEASED)) {
    if (me.getSource() == upButton) {
      if (--topViewableItem < 0) {
        topViewableItem = 0;
      }
    } else if (me.getSource() == downButton) {
      if (++topViewableItem >= (long)text.size() - maxViewableItems) {
        topViewableItem = (long)text.size() - maxViewableItems;
      }
    }
  }
}

// doActivate
//--------------------------------------------------------------------------
void CreditsScrollView::doActivate() { /* empty */
}
/*
void HelpScrollView::processEvents()
{
    if ( Desktop::getVisible("GameView") )
        COMMAND_PROCESSOR.process(false);
}
*/
