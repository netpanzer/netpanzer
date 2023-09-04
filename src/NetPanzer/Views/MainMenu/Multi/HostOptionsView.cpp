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

#include "HostOptionsView.hpp"
#include "Interfaces/GameConfig.hpp"
#include "HostView.hpp"
#include "Particles/ParticleSystemGlobals.hpp"
#include "MapSelectionView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Views/Components/Label.hpp"

#include "Views/Components/Desktop.hpp"
#include "Views/MainMenu/MenuTemplateView.hpp"

int HostOptionsView::cloudCoverageCount = 0;
int HostOptionsView::windSpeed          = 0;
int HostOptionsView::gameType           = 0;

int HostOptionsView::mapStyle           = 0;

std::string HostOptionsView::cloudCoverageString;
std::string HostOptionsView::windSpeedString;

static int getCurMaxPlayersCount()
{
    return GameConfig::game_maxplayers;
}

static int  getCurMaxUnitCount()
{
    return GameConfig::game_maxunits;
}

static void bDecreasePlayerCount()
{
    if(GameConfig::game_maxplayers - 1 >= 2)
        GameConfig::game_maxplayers = GameConfig::game_maxplayers - 1;
}

static void bIncreasePlayerCount()
{
    if(GameConfig::game_maxplayers + 1 <= 16)
        GameConfig::game_maxplayers = GameConfig::game_maxplayers + 1;
}

static void bDecreaseMaxUnitCount()
{
    if(GameConfig::game_maxunits - 5 >= 2)
        GameConfig::game_maxunits = GameConfig::game_maxunits - 5;
}

static void bIncreaseMaxUnitCount()
{
    if(GameConfig::game_maxunits + 5 <= 1000)
        GameConfig::game_maxunits = GameConfig::game_maxunits + 5;
}

void HostOptionsView::updateGameConfigCloudCoverage()
{
    // Make sure the cloud settings are the same for different size
    // maps, like clear on 128x128 should have far less clouds than
    // clear on 800x800.

    float cloudCount = MapSelectionView::mapList[MapSelectionView::curMap]->cells.getArea() / baseTileCountPerCloud;
    float randCount  = rand() % int(cloudCount / randomDivisorOfBase);

    cloudCount += randCount;

    float clearCloudCount         = float(cloudCount) * clearPercentOfBase;
    float brokenCloudCount        = float(cloudCount) * brokenPercentOfBase;
    float partlyCloudyCloudCount  = float(cloudCount) * partlyCloudyPercentOfBase;
    float overcastCloudCount      = float(cloudCount) * overcastPercentOfBase;
    float fuckingCloudyCloudCount = float(cloudCount) * extremelyCloudyPercentOfBase;

    switch (cloudCoverageCount) {
    case 0: {
            cloudCoverageString = "Clear";
            GameConfig::game_cloudcoverage = (int(clearCloudCount));
        }
        break;
    case 1: {
            cloudCoverageString = "Broken";
            GameConfig::game_cloudcoverage = (int(brokenCloudCount));
        }
        break;
    case 2: {
            cloudCoverageString = "Partly Cloudy";
            GameConfig::game_cloudcoverage = (int(partlyCloudyCloudCount));
        }
        break;
    case 3: {
            cloudCoverageString = "Overcast";
            GameConfig::game_cloudcoverage = (int(overcastCloudCount));
        }
        break;
    case 4: {
            cloudCoverageString = "Extremely Cloudy";
            GameConfig::game_cloudcoverage = (int(fuckingCloudyCloudCount));
        }
        break;
    }
}

void HostOptionsView::updateGameConfigGameType()
{
    switch (gameType) {
    case 0: {
            GameConfig::game_gametype = _gametype_objective;
        }
        break;

    case 1: {
            GameConfig::game_gametype = _gametype_fraglimit;
        }
        break;

    case 2: {
            GameConfig::game_gametype = _gametype_timelimit;
        }
        break;

    case 3: {
            GameConfig::game_gametype = _gametype_objectiveANDfraglimit;
        }
        break;

    case 4: {
            GameConfig::game_gametype = _gametype_fraglimitORtimelimit;
        }
        break;

    }

}

static const char * getGameTypeString()
{
    switch ( GameConfig::game_gametype ) {
    case _gametype_objective: {
            return( "Objective" );
        }
        break;

    case _gametype_fraglimit : {
            return( "Time Limit" );
        }
        break;

    case _gametype_timelimit : {
            return( "Frag Limit" );
        }
        break;

    case _gametype_objectiveANDfraglimit : {
            return( "Obj.+Frags" );
        }
        break;

     case _gametype_fraglimitORtimelimit : {
            return( "Frags+Time" );
        }
        break;

    }
    return( "Unknown" );
}

static void bIncreaseTimeLimit()
{
    if(GameConfig::game_timelimit + 5 <= 240)
        GameConfig::game_timelimit = GameConfig::game_timelimit + 5;
}

static void bDecreaseTimeLimit()
{
    if(GameConfig::game_timelimit - 5 >= 5)
        GameConfig::game_timelimit = GameConfig::game_timelimit - 5;
}

static int getTimeLimitHours()
{
    return GameConfig::game_timelimit / 60;
}

static int getTimeLimitMinutes()
{
    return GameConfig::game_timelimit % 60;
}

static void bIncreaseFragLimit()
{
    if(GameConfig::game_fraglimit + 5 <= 1000)
        GameConfig::game_fraglimit = GameConfig::game_fraglimit + 5;
}

static void bDecreaseFragLimit()
{
    if(GameConfig::game_fraglimit - 5 >= 5)
        GameConfig::game_fraglimit = GameConfig::game_fraglimit - 5;
}

static int getFragLimit()
{
    return GameConfig::game_fraglimit;
}

std::string HostOptionsView::getMapStyleString()
{
    return *GameConfig::game_mapstyle;
}

void HostOptionsView::updateGameConfigMapStyle()
{
    switch (mapStyle) {
    case 0: {
            *GameConfig::game_mapstyle = "SummerDay";
        }
        break;

    case 1: {
            *GameConfig::game_mapstyle = "Desert";
        }
        break;

    case 2: {
            *GameConfig::game_mapstyle = "IcyWinter";
        }
        break;

    case 3: {
            *GameConfig::game_mapstyle = "Moonlight";
        }
        break;

    case 4: {
            *GameConfig::game_mapstyle = "Scorched";
        }
        break;

    case 5: {
            *GameConfig::game_mapstyle = "Martian";
        }
        break;

    }

}





void HostOptionsView::updateWindSpeedString()
{
    float calmWindSpeed    = float(baseWindSpeed) * calmWindsPercentOfBase;
    float breezyWindSpeed  = float(baseWindSpeed) * breezyWindsPercentOfBase;
    float briskWindSpeed   = float(baseWindSpeed) * briskWindsPercentOfBase;
    float heavyWindSpeed   = float(baseWindSpeed) * heavyWindsPercentOfBase;
    float typhoonWindSpeed = float(baseWindSpeed) * typhoonWindsPercentOfBase;

    switch (windSpeed) {
    case 0: {
            windSpeedString = "Calm";
            GameConfig::game_windspeed = int(calmWindSpeed);
        }
        break;
    case 1: {
            windSpeedString = "Breezy";
            GameConfig::game_windspeed = int(breezyWindSpeed);
        }
        break;
    case 2: {
            windSpeedString = "Brisk Winds";
            GameConfig::game_windspeed = int(briskWindSpeed);
        }
        break;
    case 3: {
            windSpeedString = "Heavy Winds";
            GameConfig::game_windspeed = int(heavyWindSpeed);
        }
        break;
    case 4: {
            windSpeedString = "Typhoon";
            GameConfig::game_windspeed = int(typhoonWindSpeed);
        }
        break;
    }
}

static int getObjectiveCapturePercent()
{
    return GameConfig::game_occupationpercentage;
}

static void bIncreaseObjectiveCapturePercent()
{
    if(GameConfig::game_occupationpercentage + 5 <= 100)
        GameConfig::game_occupationpercentage =
            GameConfig::game_occupationpercentage + 5;
}

static void bDecreaseObjectiveCapturePercent()
{
    if(GameConfig::game_occupationpercentage - 5 >= 5 )
        GameConfig::game_occupationpercentage =
            GameConfig::game_occupationpercentage - 5;
}


// HostOptionsView
//---------------------------------------------------------------------------
HostOptionsView::HostOptionsView() : RMouseHackView()
{
    setSearchName("HostOptionsView");
    setTitle("Host Options");
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);

    moveTo(bodyTextRect.min.x, bodyTextRect.min.y + 205);
    resizeClientArea(bodyTextRect.getSizeX()-5, 218);

    addMeterButtons(iXY(BORDER_SPACE, BORDER_SPACE));

} // end HostOptionsView::HostOptionsView

// doDraw
//---------------------------------------------------------------------------
void HostOptionsView::doDraw(Surface &viewArea, Surface &clientArea)
{


    drawMeterInfo(clientArea, iXY(BORDER_SPACE, BORDER_SPACE));

    clientArea.bltString( 4, clientArea.getHeight() - Surface::getFontHeight(),
                    "Note: Use the right mouse button to scroll quickly.",
                    windowTextColor);

    //if (!Desktop::getVisible("GameView")) {
    //}

    View::doDraw(viewArea, clientArea);

} // end HostOptionsView::doDraw

void HostOptionsView::doDeactivate()
{
    // nothing
}

// addMeterButtons
//---------------------------------------------------------------------------
void HostOptionsView::addMeterButtons(const iXY &pos)
{
    const int yOffset          = Surface::getFontHeight();
    const int arrowButtonWidth = 16;

    int x;
    int y = pos.y;

    int xTextStart    = pos.x;
    int xControlStart = 270;

    x = xTextStart;
    add( new Label(x, y, "Max Players", windowTextColor, windowTextColorShadow, true) );
    x += xControlStart;
    addButtonCenterText(iXY(x - 1, y), arrowButtonWidth, "<", "", bDecreasePlayerCount);
    x += arrowButtonWidth + meterWidth;
    addButtonCenterText(iXY(x + 1, y), arrowButtonWidth, ">", "", bIncreasePlayerCount);
    y += yOffset;

    x = xTextStart;
    add( new Label(x, y, "Game Max Unit Count", windowTextColor, windowTextColorShadow, true) );
    x += xControlStart;
    addButtonCenterText(iXY(x - 1, y), arrowButtonWidth, "<", "", bDecreaseMaxUnitCount);
    x += arrowButtonWidth + meterWidth;
    addButtonCenterText(iXY(x + 1, y), arrowButtonWidth, ">", "", bIncreaseMaxUnitCount);
    y += yOffset;
    /*
    	x = xTextStart;
    	//addLabelShadowed(iXY(x, y), "Player Respawn Unit Count", windowTextColor, windowTextColorShadow);
    	x += xControlStart;
    	addButtonCenterText(iXY(x - 1, y), arrowButtonWidth, "<", "", bDecreaseRespawnCount);
    	x += arrowButtonWidth + meterWidth;
    	addButtonCenterText(iXY(x + 1, y), arrowButtonWidth, ">", "", bIncreaseRespawnCount);
    	y += yOffset;
    */
    //x = xTextStart;
    //addLabelShadowed(iXY(x, y), "Allow Allies", windowTextColor, windowTextColorShadow);
    //x += xControlStart;
    //addButtonCenterText(iXY(x - 1, y), arrowButtonWidth, "<", "", bSetAllowAlliesFalse);
    //x += arrowButtonWidth + meterWidth;
    //addButtonCenterText(iXY(x + 1, y), arrowButtonWidth, ">", "", bSetAllowAlliesTrue);
    //y += yOffset;
    //
    //x = xTextStart;
    //addLabelShadowed(iXY(x, y), "Spawn Placement", windowTextColor, windowTextColorShadow);
    //x += xControlStart;
    //addButtonCenterText(iXY(x - 1, y), arrowButtonWidth, "<", "", bPreviousSpawnPlacement);
    //x += arrowButtonWidth + meterWidth;
    //addButtonCenterText(iXY(x + 1, y), arrowButtonWidth, ">", "", bNextSpawnPlacement);
    //y += yOffset;

    x = xTextStart;
    add( new Label( x, y, "Objective Capture Percent", windowTextColor, windowTextColorShadow, true) );
    x += xControlStart;
    addButtonCenterText(iXY(x - 1, y), arrowButtonWidth, "<", "", bDecreaseObjectiveCapturePercent);
    x += arrowButtonWidth + meterWidth;
    addButtonCenterText(iXY(x + 1, y), arrowButtonWidth, ">", "", bIncreaseObjectiveCapturePercent);
    y += yOffset;

    x = xTextStart;
    add( new Label( x, y, "Time Limit", windowTextColor, windowTextColorShadow, true) );
    x += xControlStart;
    addButtonCenterText(iXY(x - 1, y), arrowButtonWidth, "<", "", bDecreaseTimeLimit);
    x += arrowButtonWidth + meterWidth;
    addButtonCenterText(iXY(x + 1, y), arrowButtonWidth, ">", "", bIncreaseTimeLimit);
    y += yOffset;

    x = xTextStart;
    add( new Label( x, y, "Frag Limit", windowTextColor, windowTextColorShadow, true) );
    x += xControlStart;
    addButtonCenterText(iXY(x - 1, y), arrowButtonWidth, "<", "", bDecreaseFragLimit);
    x += arrowButtonWidth + meterWidth;
    addButtonCenterText(iXY(x + 1, y), arrowButtonWidth, ">", "", bIncreaseFragLimit);
    y += yOffset + 20;

    const int minWidth = 150;
    int xChoiceOffset = 2;

    choiceGameType.setName("Game Type");
    choiceGameType.addItem("Objective");
    choiceGameType.addItem("Frag Limit");
    choiceGameType.addItem("Time Limit");
    choiceGameType.addItem("Obj.+Frags");
    choiceGameType.addItem("Frags+Time");
    choiceGameType.setMinWidth(minWidth);
    choiceGameType.setLocation(xChoiceOffset, y);
    choiceGameType.select( getGameTypeString() );
    add(&choiceGameType);
    xChoiceOffset += minWidth + 123;

    choiceCloudCoverage.setName("Cloud Coverage");
    choiceCloudCoverage.addItem("Clear");
    choiceCloudCoverage.addItem("Broken");
    choiceCloudCoverage.addItem("Partly Cloudy");
    choiceCloudCoverage.addItem("Overcast");
    choiceCloudCoverage.addItem("Extremely Cloudy");
    choiceCloudCoverage.setMinWidth(minWidth);
    choiceCloudCoverage.setLocation(xChoiceOffset, y);
    choiceCloudCoverage.select(cloudCoverageCount);
    add(&choiceCloudCoverage);
    xChoiceOffset += minWidth + 13;

    choiceWindSpeed.setName("Wind Speed");
    choiceWindSpeed.addItem("Calm");
    choiceWindSpeed.addItem("Breezy");
    choiceWindSpeed.addItem("Brisk");
    choiceWindSpeed.addItem("Heavy");
    choiceWindSpeed.addItem("Typhoon");
    choiceWindSpeed.setMinWidth(minWidth);
    choiceWindSpeed.setLocation(xChoiceOffset, y);
    choiceWindSpeed.select(windSpeed);
    add(&choiceWindSpeed);
    xChoiceOffset += minWidth + 10;

    int checkboxesPosY = y + choiceGameType.getSize().y + Surface::getFontHeight();
    checkPublic.setLabel("Public");
    checkPublic.setState(GameConfig::server_public);
    checkPublic.setLocation(2, checkboxesPosY);
    add(&checkPublic);

    checkPowerUp.setLabel("PowerUps");
    checkPowerUp.setState(GameConfig::game_powerups);
    checkPowerUp.setLocation(120, checkboxesPosY);
    add(&checkPowerUp);


    choiceMapStyle.setName("Map Style");
    choiceMapStyle.addItem("SummerDay");
    choiceMapStyle.addItem("Desert");
    choiceMapStyle.addItem("IcyWinter");
    choiceMapStyle.addItem("Moonlight");
    choiceMapStyle.addItem("Scorched");
    choiceMapStyle.addItem("Martian");
    choiceMapStyle.setMinWidth(minWidth);
    choiceMapStyle.setLocation(358, y + choiceWindSpeed.getSize().y + Surface::getFontHeight());
    choiceMapStyle.select(getMapStyleString());
    add(&choiceMapStyle);




    /*
    	x = xTextStart;
    	//addLabelShadowed(iXY(x, y), "Allow Fog Of War", windowTextColor, windowTextColorShadow);
    	x += xControlStart;
    	addButtonCenterText(iXY(x - 1, y), arrowButtonWidth, "<", "", bSetAllowFogOfWarFalse);
    	x += arrowButtonWidth + meterWidth;
    	addButtonCenterText(iXY(x + 1, y), arrowButtonWidth, ">", "", bSetAllowFogOfWarTrue);
    */
} // end HostOptionsView::addMeterButtons

// drawMeterInfo
//---------------------------------------------------------------------------
void HostOptionsView::drawMeterInfo(Surface &dest, const iXY &pos)
{
    char strBuf[256];

    int fontHeight = Surface::getFontHeight();
    const int arrowButtonWidth = 16;
    const int yOffset          = fontHeight;

    int x = pos.x + 270 + arrowButtonWidth;
    int y = pos.y;

    Surface tempSurface(meterWidth, fontHeight, 1);
    tempSurface.fill(meterColor);

    // Game Max Player Count
    tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    sprintf(strBuf, "%d", getCurMaxPlayersCount());
    tempSurface.bltStringCenter(strBuf, meterTextColor);
    tempSurface.blt(dest, x, y);

    // Game Max Unit Count
    y += yOffset;
    tempSurface.fill(meterColor);
    tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    sprintf(strBuf, "%d - %d max per player", getCurMaxUnitCount(), getCurMaxUnitCount() / getCurMaxPlayersCount());
    tempSurface.bltStringCenter(strBuf, meterTextColor);
    tempSurface.blt(dest, x, y);
    /*
    	// Respawn Unit Count
    	y += yOffset;
    	tempSurface.fill(meterColor);
    	tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    	sprintf(strBuf, "%d", getRespawnUnitCount());
    	tempSurface.bltStringCenter(strBuf, meterTextColor);
    	tempSurface.blt(dest, x, y);
    */
    // Allow Allies
    //y += yOffset;
    //tempSurface.fill(meterColor);
    //tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    //sprintf(strBuf, "%s", getAllowAllies());
    //tempSurface.bltStringCenter(strBuf, meterTextColor);
    //tempSurface.blt(dest, x, y);
    //
    //// Spawn Placement
    //y += yOffset;
    //tempSurface.fill(meterColor);
    //tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    //sprintf(strBuf, "%s", getSpawnPlacement());
    //tempSurface.bltStringCenter(strBuf, meterTextColor);
    //tempSurface.blt(dest, x, y);

    // Objective Capture Percent
    y += yOffset;
    tempSurface.fill(meterColor);
    tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    if (MapSelectionView::curMap >= 0 && MapSelectionView::mapList.size() > 0) {
        int objectiveCount =
            MapSelectionView::mapList[MapSelectionView::curMap]->objectiveCount;
        sprintf(strBuf, "%d%% - %d of %d", getObjectiveCapturePercent(),
                int(float(objectiveCount) * (float(getObjectiveCapturePercent()
                            ) / 100.0f) + 0.999), objectiveCount);
    } else {
        sprintf(strBuf, "Map Data Needed");
    }
    tempSurface.bltStringCenter(strBuf, meterTextColor);
    tempSurface.blt(dest, x, y);

    y += yOffset;
    tempSurface.fill(meterColor);
    tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    sprintf(strBuf, "%d:%d", getTimeLimitHours(), getTimeLimitMinutes() );
    tempSurface.bltStringCenter(strBuf, meterTextColor);
    tempSurface.blt(dest, x, y);

    y += yOffset;
    tempSurface.fill(meterColor);
    tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    sprintf(strBuf, "%d Frags", getFragLimit() );
    tempSurface.bltStringCenter(strBuf, meterTextColor);
    tempSurface.blt(dest, x, y);


    /*
    	// Fog of War
    	y += yOffset;
    	tempSurface.fill(meterColor);
    	tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    	sprintf(strBuf, "%s", getAllowFogOfWar());
    	tempSurface.bltStringCenter(strBuf, meterTextColor);
    	tempSurface.blt(dest, x, y);
    */

} // end HostOptionsView::drawMeterInfo

// actionPerformed
//---------------------------------------------------------------------------
void HostOptionsView::actionPerformed(mMouseEvent me)
{
    if (me.getSource()==&checkPublic) {
        if ( getVisible() ) {
            GameConfig::server_public = checkPublic.getState();
        }
    } else if (me.getSource()==&checkPowerUp) {
        if ( getVisible() ) {
            GameConfig::game_powerups = checkPowerUp.getState();
        }
    } else if (me.getSource()==&choiceWindSpeed) {
        windSpeed = choiceWindSpeed.getSelectedIndex();

        updateWindSpeedString();
    } else if (me.getSource()==&choiceCloudCoverage) {
        cloudCoverageCount = choiceCloudCoverage.getSelectedIndex();

        updateGameConfigCloudCoverage();
    } else if (me.getSource()==&choiceGameType) {
        if ( getVisible() ) {
            gameType = choiceGameType.getSelectedIndex();

            updateGameConfigGameType();
        }
    } else if (me.getSource()==&choiceMapStyle) {
        if ( getVisible() ) {
            mapStyle = choiceMapStyle.getSelectedIndex();

            updateGameConfigMapStyle();
        }
    }
} // end HostOptionsView::actionPerformed
