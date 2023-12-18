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

#include "CreditsScrollView.hpp"


// CreditsView
//---------------------------------------------------------------------------
CreditsView::CreditsView() : MenuTemplateView()
{
    setSearchName("CreditsView");
    setTitle("Credits");
    setSubTitle("");
    setAllowResize(false);
    setAllowMove(false);
    //MenuTemplateView::loadBackgroundSurface();
    //setVisible(true);

}

// doDraw
//---------------------------------------------------------------------------
void CreditsView::doDraw(Surface &viewArea, Surface &clientArea)
{

    //MenuTemplateView::doDraw(viewArea, clientArea);

    MenuTemplateView::doDraw(viewArea, clientArea);


}

void CreditsView::loadTitleSurface()
{
   // doLoadTitleSurface("CreditsTitle");
} // end HelpView::loadTitleSurface
void CreditsView::doActivate()
{
    MenuTemplateView::doActivate();
    Desktop::setVisibilityNoDoAnything("CreditsScrollView", true);
    //Desktop::setVisibility("CreditsView", true);
} // end Credits::doActivate

// doDeactivate
//---------------------------------------------------------------------------
void CreditsView::doDeactivate()
{
} // end CreditsView::doDeactivate



