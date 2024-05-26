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
#include "Views/Components/ScrollableText.hpp"
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

  std::string credits = ""
                        "This application is free software under the terms of the"
                        "Gnu General Public license (GPL). See the COPYING file for details."
                        "Source and binaries: https://github.com/netpanzer/netpanzer/"
                        "\n"
                        "\n"
                        "Website: https://netpanzer.io\n"
                        "- for binaries, maps, flags, related tools and news\n"
                        "- and don't forget to register your nickname for the ranking\n"
                        "\n"
                        "Current Development Team"
                        "- Devon Winrick (winrid)"
                        "- Andy Alt"
                        "- Fulvio Testi (fu)"
                        "\n"
                        "\n"
                        "Authors\n"
                        "\n"
                        "Additional graphics:\n"
                        "Peter Lisker (Nessie), Jesus Eugenio (Silvestre)\n"
                        "\n"
                        "\n"
                        "Original Game (Pyrosoft):\n"
                        "Vlad Rahkoy, Skip Rhudy, Matt Bogue, Clint Bogue\n"
                        "\n"
                        "Linux Port, Polishing, Packaging and Related Tools:\n"
                        "\n"
                        "Matthias Braun, Ivo Danihelka, Hollis Blanchard, Hankin Chick, \n"
                        "BenUrban Tyler Nielsen, Bastosz Fenski, Tobias Blerch, Ingo Ruhnke\n"
                        "\n"
                        "0.8.3 Release:\n"
                        "Aaron Perez (krom), C-D, fu\n"
                        "\n"
                        "0.8.4 Release up to 0.8.5-test-1:\n"
                        "Aaron Perez (krom), Laurant Jacques (Wile64), C-D, fu\n"
                        "\n"
                        "0.8.7 Release:\n"
                        "Fulvio Testi (fu), Guido Ueffing (Lohengrin)";
  scrollableText = new ScrollableText(credits, bodyTextRect.getSizeX());

}

// doDraw
//---------------------------------------------------------------------------

void CreditsScrollView::doDraw(Surface &viewArea, Surface &clientArea) {
  drawHelpText(clientArea, 0, 0);

  clientArea.bltString(4, clientArea.getHeight() - Surface::getFontHeight(),
                       "Note: Use the right mouse button to scroll quickly.",
                       windowTextColor, Color::white);

  View::doDraw(viewArea, clientArea);
}

// drawHelpText
//--------------------------------------------------------------------------
void CreditsScrollView::drawHelpText(Surface &dest, const int &, const int &) {
  scrollableText->draw(dest);
}

// doActivate
//--------------------------------------------------------------------------
void CreditsScrollView::doActivate() { /* empty */
}

void CreditsScrollView::actionPerformed(mMouseEvent me) {
  // TODO
  // TODO why not const &?
}
