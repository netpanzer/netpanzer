/*
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


#include "Views/MainMenu/Multi/TipsView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Interfaces/GameManager.hpp"
#include "Views/GameViewGlobals.hpp"

#include "Views/MainMenu/Multi/HostJoinTemplateView.hpp"
//#include "Views/MainMenu/Multi/GetSessionView.hpp"
//#include "Interfaces/GameConfig.hpp"


// CreditsView
//---------------------------------------------------------------------------
TipsView::TipsView() : View()
{
    setSearchName("TipsView");
    setTitle("Tips");
    setSubTitle("");
    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);

    moveTo(bodyTextRect.min + iXY(0, 100));
    resizeClientArea(bodyTextRect.max - bodyTextRect.min - iXY(5,380));
    tipnumber = rand() % 6;


    //setVisible(true);
}

// doDraw
//---------------------------------------------------------------------------
void TipsView::doDraw(Surface &viewArea, Surface &clientArea)
{


    //moveTo(iXY(bodyTextRect.min.x, bodyTextRect.min.y + 100));
/*
    iXY  area_size = iXY(
            50 * 8 + 16,
            Surface::getFontHeight() + 4 + 16);
    resizeClientArea(area_size);
*/
    //MenuTemplateView::doDraw(viewArea, clientArea);
    //moveTo(iXY(bodyTextRect.min.x, bodyTextRect.min.y + 80));

    //iXY  area_size = iXY(20, 120);
    //resizeClientArea(area_size);
    //moveTo(iXY(bodyTextRect.min.x, bodyTextRect.min.y + 100));

    // The plus 8 for x and 4 for y are what I put in input field.  Add function to find out,
    // inpit field dimension.

    //resizeClientArea(40 * 8 , 40);

    clientArea.fill(Color::lightGray);



        switch( tipnumber )
        {
        case 0 :
               clientArea.bltString( 4, Surface::getFontHeight(), "Missing maps? Download them from netpanzer.info website.", Color::darkGray);
               clientArea.bltString( 4, Surface::getFontHeight()+12, "Then unzip the content in your 'maps' folder.", Color::darkGray);
               break;

        case 1 :
               clientArea.bltString( 4, Surface::getFontHeight(), "You can download many flag packages from netpanzer.info.", Color::darkGray);
               clientArea.bltString( 4, Surface::getFontHeight()+12, "Then copy/paste the flags you like in your 'flag' folder.", Color::darkGray);
               clientArea.bltString( 4, Surface::getFontHeight()+24, "Or you can make your own flags!", Color::darkGray);
               break;

        case 2 :
               clientArea.bltString( 4, Surface::getFontHeight(), "When in game: press Enter to chat with other players.", Color::darkGray);
               clientArea.bltString( 4, Surface::getFontHeight()+12, "Or press TAB key to check the game status.", Color::darkGray);
               break;
        case 3 :
               clientArea.bltString( 4, Surface::getFontHeight(), "You can pick a server from the in-game Lobby.", Color::darkGray);
               clientArea.bltString( 4, Surface::getFontHeight()+12, "Or You can manually specify the IP address of the server.", Color::darkGray);
               break;
        case 4 :
               clientArea.bltString( 4, Surface::getFontHeight(), "Check the Help section for a detailed list of commands.", Color::darkGray);
               clientArea.bltString( 4, Surface::getFontHeight()+12, "When in game you can access it through F1 key.", Color::darkGray);
               break;

        case 5 :
               clientArea.bltString( 4, Surface::getFontHeight(), "By pressing Ctrl key + left mouse button you can shoot while moving.", Color::darkGray);
               clientArea.bltString( 4, Surface::getFontHeight()+12, "Always keep moving while shooting.", Color::darkGray);
               clientArea.bltString( 4, Surface::getFontHeight()+24, "That makes harder for others to hit your tanks!", Color::darkGray);
               break;

        default:

            return;
        }


    View::doDraw(viewArea, clientArea);
}

void TipsView::loadTitleSurface()
{

}


