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

#include "UStyleSelectionView.hpp"
#include "Classes/ScreenSurface.hpp"

#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/PlayerNetMessage.hpp"
#include "Classes/WorldInputCmdProcessor.hpp"

#include "Interfaces/GameManager.hpp"
#include "Classes/Network/NetworkState.hpp"
//#include "Units/UnitProfileInterface.hpp"

#include "Views/GameViewGlobals.hpp"
//#include "Resources/ResourceManager.hpp"
#include "Views/Components/Button.hpp"
#include "Views/Components/Label.hpp"
#include "Views/Components/Desktop.hpp"

#include "2D/Palette.hpp"

#include <vector>
#include <string>
#include <sstream>

#define BORDER_SPACE 4

unsigned char UStyleSelectionView::rstyle_mem;


UStyleSelectionView::UStyleSelectionView() : View()
{
    setSearchName("UStyleSelectionView");
    setTitle("UStyle Selection");
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);
    setBordered(false);

    loaded = false;
}


void UStyleSelectionView::init()
{
    removeComponents();

    iRect viewrect = getClientRect();

    //rect.min.x = (viewrect.getSizeX()/2) - 250;
    //rect.min.y = (viewrect.getSizeY()/2) - 250;
    //rect.max.x = rect.min.x + 500;
    //rect.max.y = rect.min.y + 500;


    rect.min.x = (viewrect.getSizeX()/2) - 150;
    rect.min.y = (viewrect.getSizeY()/2) - 250;
    rect.max.x = rect.min.x + 300;
    rect.max.y = rect.min.y + 500;



    iXY flagStartOffset(rect.min.x + 74, rect.min.y + 14);
    int x = flagStartOffset.x;
    int y = flagStartOffset.y;


    const char* statBuf = "Unit Style Selection";
    add( new Label(x-5, y, statBuf, Color::lightGray, Color::lightGray, false) );

    std::stringstream pnum;

    if ( NetworkState::status == _network_state_server )  // server only
    {
    pnum << (unsigned int)GameConfig::getUnitStylesNum();
    } else {
    pnum << (unsigned int)GameManager::ststylesnum;
    }




    std::string pdata;
    pdata = "(available: " + pnum.str() + ")";
    const NPString varBuf = pdata;
    add( new Label(x+18, y+16, varBuf, Color::lightGray, Color::lightGray, false) );


    const NPString b_arrow = ">>                    <<";

    if ( NetworkState::status == _network_state_server )  // server only
    {

    for (unsigned char i = 0; i < GameConfig::getUnitStylesNum(); i++) {

        NPString sstyle = GameConfig::getUnitStyle(i);
        add( Button::createSpecialButton( sstyle, sstyle, iXY(x, y+32+28*(i+1))) );
        if (i == rstyle_mem ) {
        add( new Label(x-20, y+40+28*(i+1), b_arrow, Color::lightGray, Color::lightGray, false) );
        }
    }

    } else {

    for (unsigned char i = 0; i < GameManager::ststylesnum; i++) {

        NPString sstyle = GameManager::stlist[i];
        add( Button::createSpecialButton( sstyle, sstyle, iXY(x, y+32+28*(i+1))) );
        if (i == rstyle_mem ) {
        add( new Label(x-20, y+40+28*(i+1), b_arrow, Color::lightGray, Color::lightGray, false) );
        }
    }

    }


    loaded = true;
}


void UStyleSelectionView::doDraw(Surface &viewArea, Surface &clientArea)
{
    clientArea.BltRoundRect(rect, 14, Palette::darkGray256.getColorArray());
    clientArea.RoundRect(rect,14, Color::gray);

    View::doDraw(viewArea, clientArea);
} // end doDraw


void UStyleSelectionView::doActivate()
{
    if ( ! loaded )
    {
        init();
    }
    Desktop::setActiveView(this);
	}

void UStyleSelectionView::doDeactivate()
{
    if ( ! getVisible() )
    {
        removeComponents();
        loaded = false;
    }
}


void
UStyleSelectionView::checkResolution(iXY oldResolution, iXY newResolution)
{
    resize(iXY(newResolution.x, newResolution.y));
    moveTo(iXY(0,0));
}

void UStyleSelectionView::processEvents()
{
    COMMAND_PROCESSOR.process(false);
}




void UStyleSelectionView::onComponentClicked(Component* c)
{
    std::string cname = c->getName();

    // send the style msg

    UpdatePlayerUnitStyle upuf;

    std::string custyle = cname;
    std::string scustyle = custyle.substr(7,(custyle.length() - 7));
    unsigned char rstyle = 0;


    if ( NetworkState::status == _network_state_server )  // server only
    {

    for (unsigned char i = 0; i < GameConfig::getUnitStylesNum(); i++) {
    if ( GameConfig::getUnitStyle(i) == scustyle ) {
        rstyle = i;
    }
    }
    //LOGGER.info("Sending style: %d aka %s out of %d", rstyle, scustyle.c_str(), GameConfig::getUnitStylesNum());

    } else {

    for (unsigned char i = 0; i < GameManager::ststylesnum; i++) {
    if ( GameManager::stlist[i] == scustyle ) {
        rstyle = i;
    }
    }
    //LOGGER.info("Sending style: %d aka %s out of %d", rstyle, scustyle.c_str(), GameManager::ststylesnum);



    }


    rstyle_mem = rstyle;

    upuf.player_unit_style = rstyle;

    CLIENT->sendMessage(&upuf, sizeof(upuf));

    Desktop::setVisibility("UStyleSelectionView", false);
    Desktop::setVisibility("GFlagSelectionView", true);


}


