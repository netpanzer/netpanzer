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

#include "../Game/GameView.hpp"
#include "../Components/Desktop.hpp"
#include "../GameViewGlobals.hpp"
#include "../Components/ScrollableText.hpp"
#include "MenuTemplateView.hpp"

//  HelpScrollView
//---------------------------------------------------------------------------
HelpScrollView::HelpScrollView() : MenuTemplateView() {
  setSearchName("HelpScrollView");
  setTitle("Help Information");
  setSubTitle("");

  setAllowResize(false);
  setAllowMove(false);

  moveTo(bodyTextRect.min);
  resize(bodyTextRect.getSize());


  scrollableText = new ScrollableText(this, HelpScrollView::getHelpText(), bodyTextRect, Color::black, componentBodyColor);

}  // end HelpScrollView::HelpScrollView

void HelpScrollView::doDraw(Surface &viewArea, Surface &clientArea) {
  scrollableText->draw(clientArea);
}

std::string HelpScrollView::getHelpText() {
  std::string help = ""
  "Key"
  "\n"
  "  LMC = Left Mouse Click\n"
  "  RMC = Right Mouse Click\n"
  "  RMH = Right Mouse Hold\n"
  "\n"
  "\n"
  "Unit Related"
  "\n"
  "  LMC..............................Selects a unit\n"
  "  LMH..............................Makes a bouding box for unit selection\n"
  "  RMC..............................De-selects all selected units\n"
  "  LMC + Ctrl.......................Directs (force/manual) fire of a unit\n"
  "  LMC + Shift, LMC + Ctrl..........Add unit to selection\n"
  "  Ctrl + [0..9]....................Define a group of units\n"
  "  [0..9]...........................Recall a group of units\n"
  "  [0..9] (2 times).................Center view on a group of units\n"
  "  'E'                              Select all units on screen\n"
  "  LMC + 'T'............................Track the selected unit\n"
  "  LMC + 'W'............................Set the selected objective\n"
  "  RMH..............................Fast map scroll\n"
  "  X....................................Scatter units\n"
  "  S....................................Stop units\n"
  "  LMC + Ctrl + Alt.....................Escort unit\n"
  "\n"
  "\n"
  "Game Related"
  "\n"
  "  LMC + 'A'........................Allies with the clicked unit's team\n"
  "  ESC..............................In game menus\n"
  "\n"
  "\n"
  "Outpost Related"
  "\n"
  "  'O'..............................Toggles outpost names\n"
  "  LMC on captured outpost..........Displays the outpost view\n"
  "  mouse drag on captured outpost...Select unit spawn point\n"
  "\n"
  "\n"
  "Chat Related"
  "\n"
  "  Enter............................Send message to all\n"
  "  'T'..............................Send message to allies\n"
  "  'C'..............................Send count down message\n"
  "\n"
  "\n"
  "View Related"
  "\n"
  "  F1...............................Help View\n"
  "  'U'..............................Toggles Unit selection window\n"
  "  'B'..............................Toggles Flag selection window\n"
  "  'D'..............................Damage View\n"
  "  'F'..............................Flag View\n"
  "  'M'..............................Toggles Map View\n"
  "  Shift + 'M'......................Toggles a Bigger Map View\n"
  "  'N'..............................Name View\n"
  "  TAB..............................Rank View\n"
  "  RMH + mouse movement.............Moves any view on the screen\n"
  "  RMH + [1 - 5]....................Sets the background drawing mode\n"
  "\n"
  "\n"
  "Mini Map View Related"
  "\n"
  "  RMH + [1 - 7]....................Sets the blending level of the minimap\n"
  "  RMH + [+ or -]...................Scales the size of the minimap\n"
  "  RMH + 'O'                        Toggles outposts\n"
  "\n"
  "\n"
  "System Related"
  "\n"
  "  ALT + Enter......................Toggles Fullscreen/Windowed mode\n"
  "  F9...............................BMP screen shot\n"
  "\n"
  "\n"
  "Scripting"
  "\n"
  "\n"
  "Client Related"
  "\n"
  "  /listcommands.....................List of available commands\n"
  "  /countdown [timeout in seconds]...Generate a countdown\n"
  "  /quit.............................Quit the game\n"
  "  /help.............................Show Help window (same 'F1')\n"
  "  /say [your talk]..................Send message to all (same 'Enter'\n"
  "  /teamsay [your talk]..............Send message to allies (same Ctrl + 'A')\n"
  "\n"
  "Server Related"
  "\n"
  "  /server listcommands..............List of Gameserver available commands\n"
  "  /server adminlogin [password].....Gameserver admin login\n"
  "  /server unitspawnlist [list]......List/set spawn units (number)\n"
  "  /server unitprofiles [list].......List/set spawn units (types)\n"
  "  /server help......................Show Help window (same 'F1')\n"
  "  /server map [map name]............Restart game with the given map\n"
  "  /server autokick [timeout]........Show/set autokick for inactive players\n"
  "  /server flagtimer [timeout].......Show/set flag change timer\n"
  "  /server countdown [timeout].......Generate a countdown (same 'c')\n"
  "  /server say [your talk]...........Server say something in chat\n"
  "  /server listplayers...............List players with the assigned number\n"
  "  /server listprofiles..............List available profiles\n"
  "  /server kick [player number]......Kick a given player\n"
  "  /server baselimit [max bases].....Show/set the max bases number per player\n"
  "  /server gamepass [password].......Show/set game connection password";

  return help;
}

void HelpScrollView::doActivate() { /* empty */
}

void HelpScrollView::actionPerformed(mMouseEvent me) {
  scrollableText->actionPerformed(me);
}
