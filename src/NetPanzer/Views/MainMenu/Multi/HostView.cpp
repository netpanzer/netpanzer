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


#include "HostView.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Views/GameViewGlobals.hpp"
#include "HostOptionsView.hpp"

/////////////////////////////////////////////////////////////////////////////
// Buttons functions.
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
HostView::HostView() : HostJoinTemplateView()
{
    setSearchName("HostView");
    setTitle("Host Options");
    setSubTitle("");

    sprintf(HostView::gameTypeBuf, "Objective");

    //addButtonCenterText(iXY(x, y), buttonXSize, "Objective", "", setGameTypeObjective);

    //x += buttonXSize;
    //addButtonCenterText(iXY(x, y), buttonXSize, "Frag Limit", "", setGameTypeFragLimit);
    //addButton(iXY(x + buttonXSize + 1, y), "+", "", increaseFragLimit);
    //addButton(iXY(x + buttonXSize + 15, y), "-", "", decreaseFragLimit);

    //x += buttonXSize;
    //addButtonCenterText(iXY(x, y), buttonXSize, "Time Limit", "", setGameTypeTimeLimit);
    //addButton(iXY(x + buttonXSize + 1, y), "+", "", increaseTimeLimit);
    //addButton(iXY(x + buttonXSize + 15, y), "-", "", decreaseTimeLimit);

    //iXY pos(200, 380);
    //int yOffset = 20;
    //choiceAllowAllies.add("Yes");
    //choiceAllowAllies.add("No");
    //choiceAllowAllies.setLocation(pos);
    //pos.y += yOffset;
    //add(&choiceAllowAllies);
    //
    //choiceSpawnPlacement.add("Round Robin");
    //choiceSpawnPlacement.add("Random");
    //choiceSpawnPlacement.setLocation(pos);
    //pos.y += yOffset;
    //add(&choiceSpawnPlacement);
    //
    //choiceCloudCoverage.add("Clear");
    //choiceCloudCoverage.add("Broken");
    //choiceCloudCoverage.add("Partly Cloudy");
    //choiceCloudCoverage.add("Overcast");
    //choiceCloudCoverage.add("Extremely Cloudy");
    //choiceCloudCoverage.setLocation(pos);
    //pos.y += yOffset;
    //add(&choiceCloudCoverage);
    //
    //choiceWindSpeed.add("Calm");
    //choiceWindSpeed.add("Breezy");
    //choiceWindSpeed.add("Brisk Winds");
    //choiceWindSpeed.add("Heavy Winds");
    //choiceWindSpeed.add("Typhoon");
    //choiceWindSpeed.setLocation(pos);
    //pos.y += yOffset;
    //add(&choiceWindSpeed);

} // end HostView::HostView


// doDraw
//---------------------------------------------------------------------------
void HostView::doDraw(Surface &viewArea, Surface &clientArea)
{
    HostJoinTemplateView::doDraw(viewArea, clientArea);

    /*
    	char strBuf[256];
     
    	sprintf(strBuf, "Game Type: %s", gameTypeBuf);
    	clientArea.bltString( x + 2, y, strBuf, Color::white);
    	y += yOffset;
    	y += yOffset;
     
    	sprintf(strBuf, "%d frags", gameconfig->GetFragLimit());
    	clientArea.bltString( x + 220, y, strBuf, Color::white);
    	y += yOffset;
     
    	if ((gameconfig->GetTimeLimit() / 60) > 0)
    	{
    		sprintf(strBuf, "%d hours  %2d minutes", gameconfig->GetTimeLimit() / 60, gameconfig->GetTimeLimit() % 60);
    		clientArea.bltString( x + 220, y, strBuf, Color::white);
    		y += yOffset;
    	} else
    	{
    		sprintf(strBuf, "%d minutes", gameconfig->GetTimeLimit());
    		clientArea.bltString( x + 220, y, strBuf, Color::white);
    		y += yOffset;
    	}
     
    	sprintf(strBuf, "Player #:");
    	clientArea.bltString(x + 2, y, strBuf, Color::white);
     
    	sprintf(strBuf, "%3d", gameconfig->GetNumberPlayers());
    	clientArea.bltString(x + 125, y, strBuf, Color::white);
    */
//    View::doDraw(viewArea, clientArea);

} // end HostView::doDraw

// loadTitleSurface
//---------------------------------------------------------------------------
void HostView::loadTitleSurface()
{
    doLoadTitleSurface("hostTitle");

} // end HostView::loadTitleSurface
