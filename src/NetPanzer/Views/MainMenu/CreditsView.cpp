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


#include "CreditsView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Interfaces/GameManager.hpp"
#include "Views/GameViewGlobals.hpp"

#include "Views/MainMenu/Multi/HostJoinTemplateView.hpp"
#include "Views/MainMenu/Multi/GetSessionView.hpp"
#include "Interfaces/GameConfig.hpp"


// CreditsView
//---------------------------------------------------------------------------
CreditsView::CreditsView() : MenuTemplateView()
{
    setSearchName("CreditsView");
    setTitle("Credits");
    setSubTitle("");

    setVisible(true);
}

// doDraw
//---------------------------------------------------------------------------
void CreditsView::doDraw(Surface &viewArea, Surface &clientArea)
{
    MenuTemplateView::doDraw(viewArea, clientArea);

    static char text[] =
        "This is NetPanzer version " PACKAGE_VERSION
        ", a massively multiplayer tank battle game.  "
        "This application is free software under the terms of the "
        "Gnu General Public license (GPL). See the COPYING file for details.\n\n"
        "Source and binaries at: http://netpanzer.sourceforge.net/\n\n"
        "Please visit www.NETPANZER.info\n"
        "- for binaries, maps, flags, related tools and news\n"
        "- and don't forget to register your nickname for the monthly ranking\n\n\n"
        "Current Team\n\n"
        "   Fulvio Testi (fu)\n"
        "   Guido Ueffing (Lohengrin)\n"
        "\n\n\n"
        "Authors\n\n"
        "Original Game (Pyrosoft):\n"
        "   Vlad Rahkoy, Skip Rhudy, Matt Bogue, Clint Bogue\n\n"
        "Linux Port, Polishing, Packaging and Related Tools:\n"
        "   Matthias Braun, Ivo Danihelka, Hollis Blanchard, Hankin Chick, BenUrban\n"
        "   Tyler Nielsen, Bastosz Fenski, Tobias Blerch, Ingo Ruhnke\n\n"
        "0.8.3 Release:\n"
        "   Aaron Perez (krom), C-D, fu\n\n"
        "0.8.4 Release up to 0.8.5-test-1:\n"
        "   Aaron Perez (krom), Laurant Jacques (Wile64), C-D, fu\n\n";

    viewArea.bltStringInBox(bodyTextRect, text, windowTextColor, 12);
}

void CreditsView::loadTitleSurface()
{
   // doLoadTitleSurface("CreditsTitle");
} // end HelpView::loadTitleSurface
void CreditsView::doActivate()
{
    //MenuTemplateView::doActivate();
    //Desktop::setVisibility("CreditsView", true);
} // end Credits::doActivate

// doDeactivate
//---------------------------------------------------------------------------
void CreditsView::doDeactivate()
{
} // end CreditsView::doDeactivate



