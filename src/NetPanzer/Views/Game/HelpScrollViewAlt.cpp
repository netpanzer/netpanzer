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
#include "GameView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Views/Components/Desktop.hpp"
#include "Classes/WorldInputCmdProcessor.hpp"

// HelpScrollView
//---------------------------------------------------------------------------
HelpScrollViewAlt::HelpScrollViewAlt() : SpecialButtonView()
{
    setSearchName("HelpScrollViewAlt");
    setTitle("Help Information");
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);

    moveTo(bodyTextRect.min);
    resize(bodyTextRect.getSize());
    //moveTo(iXY(0, 0));
    //resize(iXY(800, 600));


    insert("Key");
    insert("");
    insert("  LMC = Left Mouse Click");
    insert("  RMC = Right Mouse Click");
    insert("  RMH = Right Mouse Hold");
    insert("");
    insert("");
    insert("Unit Related");
    insert("");
    insert("  LMC                              Selects a unit");
    insert("  LMH                              Makes a bouding box for unit selection");
    insert("  RMC                              De-selects all selected units");
    insert("  LMC + Ctrl                       Directs (force/manual) fire of a unit");
    insert("  LMC + Shift, LMC + Ctrl          Add unit to selection");
    insert("  Ctrl + [0..9]                    Define a group of units");
    insert("  [0..9]                           Recall a group of units");
    insert("  [0..9] (2 times)                 Center view on a group of units");
    //insert("  'E'                              Select all units on screen");

    //insert("  LMC + 'T'............................Track the selected unit");
    //insert("  LMC + 'W'............................Set the selected objective's way point");
    insert("  RMH                              Fast map scroll");
    //insert("  X....................................Scatter units");
    //insert("  S....................................Stop units");
    //insert("  LMC + Ctrl + Alt.....................Escort unit");
    insert("");
    insert("");
    insert("Game Related");
    insert("");
    insert("  LMC + 'A'                        Allies with the clicked unit's team");
    insert("  ESC                              In game menus");
    insert("");
    insert("");
    insert("Outpost Related");
    insert("");
    insert("  'O'                              Toggle outpost names");
    insert("  LMC on captured outpost          Displays the outpost view");
    insert("  mouse drag on captured outpost   Select unit spawn point");
    insert("");
    insert("");
    insert("Chat Related");
    insert("");
    insert("  Enter                            Send message to all");
    insert("  'T'                              Send message to allies");
    insert("  'C'                              Send count down message");
    insert("");
    insert("");
    insert("View Related");
    insert("");
    insert("  F1                               Help View");
    insert("  'U'                              Toggles Unit selection window");
    insert("  'B'                              Toggles Flag selection window");
    insert("  'D'                              Damage View");
    insert("  'F'                              Flag View");
    insert("  'M'                              Toggles Map View");
    insert("  Shift + 'M'                      Toggles a Bigger Map View");
    insert("  'N'                              Name View");
    insert("  TAB                              Rank View");
    insert("  RMH + mouse movement             Moves any view on the screen");
    insert("  RMH + [1 - 5]                    Sets the background drawing mode");
    insert(" ");
    insert("Mini Map View Related");
    insert("");
    insert("  RMH + [1 - 7]                    Sets the blending level of the minimap");
    insert("  RMH + [+ or -]                   Scales the size of the minimap");
    //insert("  RMH + 'O'                        Toggles outposts");
    insert(" ");
    insert(" ");
    insert("System Related");
    insert("");
    insert("  ALT + Enter                      Toggles Fullscreen/Windowed mode");
    insert("  F9                               BMP screen shot");
    insert(" ");
    insert(" ");
    insert("Scripting");
    insert(" ");
    insert("");
    insert("Client Related");
    insert("");
    insert("  /listcommands                     List of available commands");
    insert("  /countdown [timeout in seconds]   Generate a countdown");
    insert("  /quit                             Quit the game");
    insert("  /help                             Show Help window (same 'F1')");
    insert("  /say [your talk]                  Send message to all (same 'Enter'");
    insert("  /teamsay [your talk]              Send message to allies (same Ctrl + 'A')");
    insert("");
    insert("Server Related");
    insert("");
    insert("  /server listcommands              List of Gameserver available commands");
    insert("  /server adminlogin [password]     Gameserver admin login");
    insert("  /server unitspawnlist [list]      List/set spawn units (number)");
    insert("  /server unitprofiles [list]       List/set spawn units (types)");
    insert("  /server help                      Show Help window (same 'F1')");
    insert("  /server map [map name]            Restart game with the given map");
    insert("  /server autokick [timeout]        Show/set autokick for inactive players");
    insert("  /server flagtimer [timeout]       Show/set flag change timer");
    insert("  /server countdown [timeout]       Generate a countdown (same 'c')");
    insert("  /server say [your talk]           Server say something in chat");
    insert("  /server listplayers               List players with the assigned number");
    insert("  /server listprofiles              List available profiles");
    insert("  /server kick [player number]      Kick a given player");
    insert("  /server baselimit [max bases]     Show/set the max bases number per player");
    insert("  /server gamepass [password]       Show/set game connection password");
    insert(" ");
    insert(" ");
    //insert("  Alt + '-'                        Decrease brightness");
    //insert("  Alt + '='                        Increase brightness");

    int CHAR_YPIX = Surface::getFontHeight();
    maxViewableItems = (getClientRect().getSizeY() - (TEXT_GAP_SPACE + CHAR_YPIX)) / (TEXT_GAP_SPACE + CHAR_YPIX) - 1;
    topViewableItem  = 0;

    iXY size(20, 20);
    iXY pos(getClientRect().getSizeX() - size.x, 0);

    upButton = new Button("upButton");
    upButton->setLabel("+");
    upButton->setLocation(pos.x, pos.y);
    upButton->setSize(size.x, size.y);
    upButton->setNormalBorder();
    upButton->setTextColors(Color::darkGray, Color::red, Color::gray);
    add(upButton);

    pos = iXY(getClientRect().getSizeX() - size.x, getClientRect().getSizeY() - size.y);
    downButton = new Button("downButton");
    downButton->setLabel("-");
    downButton->setLocation(pos.x, pos.y);
    downButton->setSize(size.x, size.y);
    downButton->setNormalBorder();
    downButton->setTextColors(Color::darkGray, Color::red, Color::gray);
    add(downButton);

} // end HelpScrollView::HelpScrollView

// doDraw
//---------------------------------------------------------------------------

void HelpScrollViewAlt::doDraw(Surface &viewArea, Surface &clientArea)
{
    if (Desktop::getVisible("GameView")) {
        bltViewBackground(viewArea);
    }



    //clientArea.FillRoundRect(getClientRect(), 10, Color::darkGray);
    drawHelpText(clientArea, 0, 0);

    clientArea.bltString(   4,
                            clientArea.getHeight() - Surface::getFontHeight(),
                            "Note: Use the right mouse button to scroll quickly.",
                            Color::lightGray);
    //char strBuf[256];
    //sprintf(strBuf, "%d", scrollBar->getValue());
    //clientArea.bltStringCenter(strBuf, Color::red);

    View::doDraw(viewArea, clientArea);

} // end HelpScrollView::doDraw


// drawHelpText
//--------------------------------------------------------------------------
void HelpScrollViewAlt::drawHelpText(Surface &dest, const int &, const int &)
{
    //PIX color   = windowTextColor;
    PIX color   = Color::white;
    //if (scrollBar != 0)
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
        dest.bltString(1, 6 + curIndex * (TEXT_GAP_SPACE +
                    Surface::getFontHeight()), text[i].c_str(), color);
        curIndex++;
    }
    //}

} // end HelpScrollView::drawHelpText

// insert
//--------------------------------------------------------------------------
void HelpScrollViewAlt::insert(const char *string)
{
    text.push_back(std::string(string));
} // end HelpScrollView::insert

// actionPerformed
//--------------------------------------------------------------------------
void HelpScrollViewAlt::actionPerformed(mMouseEvent me)
{
    if ((me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED) ||
        (me.getID() == mMouseEvent::MOUSE_EVENT_RELEASED))
    {
        if (me.getSource()==upButton) {
            if (--topViewableItem < 0) {
                topViewableItem = 0;
            }
        } else if (me.getSource()==downButton) {
            if (++topViewableItem >= (long) text.size() - maxViewableItems) {
                topViewableItem = (long) text.size() - maxViewableItems;
            }
        }
    }

} // end HelpScrollView::actionPerformed

// doActivate
//--------------------------------------------------------------------------
void HelpScrollViewAlt::doActivate()
{
    /* empty */
} // end HelpScrollView::doActivate

void HelpScrollViewAlt::processEvents()
{
    if ( Desktop::getVisible("GameView") )
        COMMAND_PROCESSOR.process(false);
}

