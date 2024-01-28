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

#include "HelpScrollView.hpp"

#include "GameView.hpp"
#include "Localization.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/GameViewGlobals.hpp"
// #include "Classes/WorldInputCmdProcessor.hpp"
//  HelpScrollView
//---------------------------------------------------------------------------
HelpScrollView::HelpScrollView() : SpecialButtonView() {
  setSearchName("HelpScrollView");
  setTitle("Help Information");
  setSubTitle("");

  setAllowResize(false);
  setAllowMove(false);

  moveTo(bodyTextRect.min);
  resize(bodyTextRect.getSize());
  // moveTo(iXY(0, 0));
  // resize(iXY(800, 600));

  // TODO - with new variable spaced font support - render two columns - keys on
  // left and descriptions on right.
  insert(_("Key"));
  insert("");
  insert(_("  LMC = Left Mouse Click"));
  insert(_("  RMC = Right Mouse Click"));
  insert(_("  RMH = Right Mouse Hold"));
  insert("");
  insert("");
  insert(_("Unit Related"));
  insert("");
  insert(_("  LMC..............................Selects a unit"));
  insert(
      _("  LMH..............................Makes a bounding box for unit selection"));
  insert(_("  RMC..............................De-selects all selected units"));
  insert(
      _("  LMC + Ctrl.......................Directs (force/manual) fire of a unit"));
  insert(_("  LMC + Shift, LMC + Ctrl..........Add unit to selection"));
  insert(_("  Ctrl + [0..9]....................Define a group of units"));
  insert(_("  [0..9]...........................Recall a group of units"));
  insert(_("  [0..9] (2 times).................Center view on a group of units"));
  // insert("  'E'                              Select all units on screen");

  // insert("  LMC + 'T'............................Track the selected unit");
  // insert("  LMC + 'W'............................Set the selected objective's
  // way point");
  insert(_("  RMH..............................Fast map scroll"));
  // insert("  X....................................Scatter units"));
  // insert("  S....................................Stop units"));
  // insert("  LMC + Ctrl + Alt.....................Escort unit"));
  insert("");
  insert("");
  insert(_("Game Related"));
  insert("");
  insert(
      _("  LMC + 'A'........................Allies with the clicked unit's team"));
  insert(_("  ESC..............................In game menus"));
  insert("");
  insert("");
  insert(_("Outpost Related"));
  insert("");
  insert(_("  'O'..............................Toggles outpost names"));
  insert(_("  LMC on captured outpost..........Displays the outpost view"));
  insert(_("  mouse drag on captured outpost...Select unit spawn point"));
  insert("");
  insert("");
  insert(_("Chat Related"));
  insert("");
  insert(_("  Enter............................Send message to all"));
  insert(_("  'T'..............................Send message to allies"));
  insert(_("  'C'..............................Send count down message"));
  insert("");
  insert("");
  insert(_("View Related"));
  insert("");
  insert(_("  F1...............................Help View"));
  insert(_("  'U'..............................Toggles Unit selection window"));
  insert(_("  'B'..............................Toggles Flag selection window"));
  insert(_("  'D'..............................Damage View"));
  insert(_("  'F'..............................Flag View"));
  insert(_("  'M'..............................Toggles Map View"));
  insert(_("  Shift + 'M'......................Toggles a Bigger Map View"));
  insert(_("  'N'..............................Name View"));
  insert(_("  TAB..............................Rank View"));
  insert(_("  RMH + mouse movement.............Moves any view on the screen"));
  insert(_("  RMH + [1 - 5]....................Sets the background drawing mode"));
  insert(" ");
  insert(" ");
  insert(_("Mini Map View Related"));
  insert("");
  insert(
      _("  RMH + [1 - 7].............Sets the blending level of the minimap"));
  insert(_("  RMH + [+ or -]...................Scales the size of the minimap"));
  // insert("  RMH + 'O'                        Toggles outposts"));
  insert(" ");
  insert(" ");
  insert(_("System Related"));
  insert("");
  insert(_("  ALT + Enter......................Toggles Fullscreen/Windowed mode"));
  insert(_("  F9...............................BMP screen shot"));
  insert(" ");
  insert(" ");
  insert(_("Scripting"));
  insert(" ");
  insert("");
  insert(_("Client Related"));
  insert("");
  insert(_("  /listcommands.....................List of available commands"));
  insert(_("  /countdown [timeout in seconds]...Generate a countdown"));
  insert(_("  /quit.............................Quit the game"));
  insert(_("  /help.............................Show Help window (same 'F1')"));
  insert(
      _("  /say [your talk]..................Send message to all (same 'Enter'"));
  insert(
      _("  /teamsay [your talk]..............Send message to allies (same Ctrl + 'A')"));
  insert("");
  insert(_("Server Related"));
  insert("");
  insert(
      _("  /server listcommands..............List of Gameserver available "
      "commands"));
  insert(_("  /server adminlogin [password].....Gameserver admin login"));
  insert(_("  /server unitspawnlist [list]......List/set spawn units (number)"));
  insert(_("  /server unitprofiles [list].......List/set spawn units (types)"));
  insert(_("  /server help......................Show Help window (same 'F1')"));
  insert(_("  /server map [map name]............Restart game with the given map"));
  insert(
      _("  /server autokick [timeout]........Show/set autokick for inactive players"));
  insert(_("  /server flagtimer [timeout].......Show/set flag change timer"));
  insert(_("  /server countdown [timeout].......Generate a countdown (same 'c')"));
  insert(_("  /server say [your talk]...........Server say something in chat"));
  insert(
      _("  /server listplayers...............List players with the assigned number"));
  insert(_("  /server listprofiles..............List available profiles"));
  insert(_("  /server kick [player number]......Kick a given player"));
  insert(
      _("  /server baselimit [max bases].....Show/set the max bases number per player"));
  insert(
      _("  /server gamepass [password].......Show/set game connection password"));
  insert(" ");
  insert(" ");
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

}  // end HelpScrollView::HelpScrollView

// doDraw
//---------------------------------------------------------------------------

void HelpScrollView::doDraw(Surface &viewArea, Surface &clientArea) {
  // if (Desktop::getVisible("GameView")) {
  //     bltViewBackground(viewArea);
  // }

  // clientArea.FillRoundRect(getClientRect(), 10, Color::darkGray);
  drawHelpText(clientArea, 0, 0);

  clientArea.bltString(4, clientArea.getHeight() - Surface::getFontHeight(),
                       _("Note: Use the right mouse button to scroll quickly."),
                       windowTextColor);
  // char strBuf[256];
  // sprintf(strBuf, "%d", scrollBar->getValue());
  // clientArea.bltStringCenter(strBuf, Color::red);

  View::doDraw(viewArea, clientArea);

}  // end HelpScrollView::doDraw

// drawHelpText
//--------------------------------------------------------------------------
void HelpScrollView::drawHelpText(Surface &dest, const int &, const int &) {
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
                   text[i].c_str(), color);
    curIndex++;
  }
  //}

}  // end HelpScrollView::drawHelpText

// insert
//--------------------------------------------------------------------------
void HelpScrollView::insert(const char *string) {
  text.push_back(std::string(string));
}  // end HelpScrollView::insert

// actionPerformed
//--------------------------------------------------------------------------
void HelpScrollView::actionPerformed(mMouseEvent me) {
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

}  // end HelpScrollView::actionPerformed

// doActivate
//--------------------------------------------------------------------------
void HelpScrollView::doActivate() {
  /* empty */
}  // end HelpScrollView::doActivate
/*
void HelpScrollView::processEvents()
{
    if ( Desktop::getVisible("GameView") )
        COMMAND_PROCESSOR.process(false);
}
*/
