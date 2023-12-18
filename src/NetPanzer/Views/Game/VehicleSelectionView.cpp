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

#include "VehicleSelectionView.hpp"

#include "Classes/Network/TerminalNetMesg.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/NetworkState.hpp"

#include "Classes/WorldInputCmdProcessor.hpp"
#include "Classes/ScreenSurface.hpp"

#include "Util/Math.hpp"
#include "Util/Exception.hpp"

#include "Interfaces/WorldViewInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/GameConfig.hpp"

#include "Objectives/ObjectiveInterface.hpp"
#include "Objectives/Objective.hpp"

#include "Units/UnitProfileInterface.hpp"
#include "Units/UnitInterface.hpp"
#include "Units/UnitTypes.hpp"

#include "Views/GameViewGlobals.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Components/ViewGlobals.hpp"
#include "Views/Components/Label.hpp"



int vsvSelectedUnit   = 0;
int vsvUnitGenOn      = true;
bool changeMade       = false;
ObjectiveID CURRENT_SELECTED_OUTPOST_ID;

static void sendOutpostStatus()
{
    if (!changeMade) {
        return;
    }

    ObjectiveInterface::sendChangeGeneratingUnit(CURRENT_SELECTED_OUTPOST_ID,
                                                 (char)vsvSelectedUnit,
                                                 vsvUnitGenOn);
}

static void bOK()
{
    Desktop::setVisibilityNoDoAnything("VehicleSelectionView", false);
    sendOutpostStatus();
    changeMade = false;
}

static void bSetPowerOn()
{
    vsvUnitGenOn = true;
    changeMade   = true;

    VehicleSelectionView::setPowerOn();
}

class UnitSelectionButton : public Button
{
protected:
    unsigned short unitType;
    VehicleSelectionView * p;
public:
    UnitSelectionButton(VehicleSelectionView * vsv, const char *cname, int unit_type, int x, int y, const Surface &s)
        : Button(cname), unitType(unit_type), p(vsv)
    {
        bimage.copy(s);
        setSize( bimage.getWidth()-1, bimage.getHeight()-1);
        setLocation(x, y);
        setUnitSelectionBorder();
    }

    void actionPerformed( const mMouseEvent &e )
    {
        if ( e.getID() == mMouseEvent::MOUSE_EVENT_PRESSED )
        {
            resetState();
            if (vsvSelectedUnit == unitType && vsvUnitGenOn) {
                Desktop::setVisibilityNoDoAnything("VehicleSelectionView", false);
                return;
            }

            vsvSelectedUnit = unitType;
            changeMade      = true;
            bSetPowerOn();
            bOK();
        }
        else if ( e.getID() == mMouseEvent::MOUSE_EVENT_ENTERED )
        {
            Button::actionPerformed(e);
            p->highlightedUnitType = unitType;
        }
        else if ( e.getID() == mMouseEvent::MOUSE_EVENT_EXITED )
        {
            Button::actionPerformed(e);
            if ( p->highlightedUnitType == unitType )
                p->highlightedUnitType = -1;
        }
    }
};

Button * VehicleSelectionView::buttonAbandonS = 0;
Button * VehicleSelectionView::buttonAbandonM = 0;
Button * VehicleSelectionView::buttonPower = 0;
Button * VehicleSelectionView::buttonOk = 0;

iRect   VehicleSelectionView::miniProductionRect(0, 0, 0, 0);
Surface VehicleSelectionView::unitImages;
bool    VehicleSelectionView::displayMiniProductionStatus = true;
bool    VehicleSelectionView::displayOutpostNames = true;



void activateVehicleSelectionView(ObjectiveID outpost_id)
{
    CURRENT_SELECTED_OUTPOST_ID = outpost_id;

    Objective* obj = ObjectiveInterface::getObjective(outpost_id);

    vsvSelectedUnit = obj->unit_generation_type;
    vsvUnitGenOn    = obj->unit_generation_on_flag;

    if (vsvUnitGenOn) {
        VehicleSelectionView::setPowerOn();
    } else {
        VehicleSelectionView::setPowerOff();
    }

    Desktop::setVisibility( "VehicleSelectionView", true);
}

void toggleDisplayOutpostNames( void )
{
    VehicleSelectionView::displayOutpostNames = !VehicleSelectionView::displayOutpostNames;
}

// VehicleSelectionView
//---------------------------------------------------------------------------
VehicleSelectionView::VehicleSelectionView() : GameTemplateView()
{
    setSearchName("VehicleSelectionView");
    setTitle("Unit Production");
    setSubTitle("");

    setAllowResize(false);
    setVisible(false);
    setBordered(true);
    setDisplayStatusBar(true);

    moveTo(iXY(0, 0));

    const int yOffset  = 16;
    const int gapSpace =  1;

    resizeClientArea(48 * 5 + gapSpace+1 * 4, 198 + 100);

    // Power status.
    iXY pos(0 ,2);

    pos.x = 0;
    add( new Label( pos.x+2, pos.y+2, "Abandon Obj.:", Color::white) );

    pos.x = getClientRect().getSizeX() - 102;
    if ( !buttonAbandonS )
        buttonAbandonS = new Button( "ButtonAbandonS");
    buttonAbandonS->setTextColors(Color::red,Color::darkRed,Color::lightGray);
    buttonAbandonS->setLabel("This");
    buttonAbandonS->setLocation(pos.x, pos.y);
    buttonAbandonS->setSize( 58, 14);
    buttonAbandonS->setNormalBorder();
    add(buttonAbandonS);

        pos.x = getClientRect().getSizeX() - 40;
    if ( !buttonAbandonM )
        buttonAbandonM = new Button( "ButtonAbandonM");
    buttonAbandonM->setTextColors(Color::red,Color::darkRed,Color::lightGray);
    buttonAbandonM->setLabel("All");
    buttonAbandonM->setLocation(pos.x, pos.y);
    buttonAbandonM->setSize( 38, 14);
    buttonAbandonM->setNormalBorder();
    add(buttonAbandonM);



    pos.y += yOffset+2;

    pos.x = 0;
    add( new Label( pos.x+2, pos.y+2, "Power:", Color::white) );

    pos.x = getClientRect().getSizeX() - 102;
    if ( !buttonPower )
        buttonPower = new Button( "ButtonPower");
    buttonPower->setTextColors(Color::black,Color::red,Color::darkGray);
    buttonPower->setLabel("Off");
    buttonPower->setLocation(pos.x,pos.y);
    buttonPower->setSize( 100, 14);
    buttonPower->setNormalBorder();
    add(buttonPower);
    pos.y += yOffset+5;

    // XXX hardcoded for now
    int CHAR_XPIX = 8;

    pos.x = 0;
    add( new Label( pos.x+2, pos.y+2, "Production:", Color::white) );
    productionUnitPos.x = (strlen("Current Unit:") + 1) * CHAR_XPIX + 2;
    productionUnitPos.y = pos.y + 2;
    pos.y += yOffset;

    pos.x = 0;
    add( new Label( pos.x+2, pos.y+2, "Time:        ", Color::white) );
    timeRequiredPos.x = (strlen("Time:        ") + 1) * CHAR_XPIX + 2;
    timeRequiredPos.y = pos.y + 2;
    pos.y += yOffset;

    pos.x = 0;
    add( new Label( pos.x+2, pos.y+2, "Unit Built:   ", Color::white) );
    unitsBuiltPos.x = (strlen("Time:        ") + 1) * CHAR_XPIX + 2;
    unitsBuiltPos.y = pos.y + 2;
    pos.y += yOffset;

    pos.x = 0;

    Surface tempSurface;

    UnitSelectionButton *usb;
    UnitProfile *uprofile;


    unitImages.create(48, 48, UnitProfileInterface::getNumUnitTypes());
    // XXX order by something?



    unsigned int typeslimit;

    typeslimit = UnitProfileInterface::getNumUnitTypes();



    for ( unsigned int ut=0; ut < typeslimit; ut++)
    {

        uprofile = UnitProfileInterface::getUnitProfile(ut);


        tempSurface.loadBMP(uprofile->imagefile.c_str());
        unitImages.setFrame(ut);
        tempSurface.blt(unitImages, 0, 0);

        usb = new UnitSelectionButton(this, uprofile->unitname.c_str(),
                                      ut, pos.x, pos.y, tempSurface);

        add(usb);

        pos.x += 48 + gapSpace;
        if ( pos.x+48 > max.x-min.x )
        {
            pos.x = 0;
            pos.y += 48 + gapSpace;
        }
    }

    if ( pos.x == 0 )
    {
        pos.y += gapSpace * 3; // and the one just added before = 4
    }
    else
    {
        pos.y += 48 + gapSpace * 4;
        pos.x = 0;
    }

    unitProfileDataY = pos.y;
    pos.y += 73;

    //addLabel(pos + 2, "Time Remaining:", Color::white);
    //timeRemainingPos.x = (strlen("Time Remaining: ") + 1) * CHAR_XPIX + 2;
    //timeRemainingPos.y = pos.y + 2;
    //pos.y += yOffset;

    pos.x = (getClientRect().getSizeX() - 100) / 2;
    if ( !buttonOk )
        buttonOk = new Button( "buttonClose");
    buttonOk->setTextColors(Color::black,Color::red,Color::darkGray);
    buttonOk->setLabel("Close");
    buttonOk->setLocation(pos.x,pos.y);
    buttonOk->setSize(100, 14);
    buttonOk->setNormalBorder();
    add(buttonOk);

    maxHitPoints = 1;
    maxAttackFactor = 1;
    maxAttackRange = 1;
    maxDefendRange = 1;
    maxTotalSpeed = 1;
    maxReloadTime = 1;
    maxRegenTime = 1;
    getProfileData();

    highlightedUnitType = -1;

} // end VehicleSelectionView::VehicleSelectionView

// doDraw
//--------------------------------------------------------------------------
void VehicleSelectionView::doDraw(Surface &viewArea, Surface &clientArea)
{
    Objective* obj = ObjectiveInterface::getObjective(CURRENT_SELECTED_OUTPOST_ID);
    if ( !obj || obj->occupying_player != PlayerInterface::getLocalPlayer() )
    {
        Desktop::setVisibilityNoDoAnything("VehicleSelectionView", false);
        changeMade = false;
        return;
    }

    char strBuf[256];

    const int color = Color::white;

    // Draw a line attaching the VehicleSelectionView to its outpost.
    {if (WorldInputCmdProcessor::isObjectiveSelected())
        {
            // Draw a line connecting the vehicleSelectionView and the objective.
            iRect gameViewRect;
            WorldViewInterface::getViewWindow(&gameViewRect);

            iXY objectivePos(WorldInputCmdProcessor::getSelectedObjectiveWorldPos());
            objectivePos -= gameViewRect.min;

            iXY a(VehicleSelectionView::min + VehicleSelectionView::getSize() / 2);
            iXY b(objectivePos);

            // Calculate the starting point on the outside of the vehicleSelectionView box.
            fXY v2oSlope(Math::unitDirection(a, b));

            a.x += int(v2oSlope.x * float(VehicleSelectionView::getSizeX() / 2));
            a.y += int(v2oSlope.y * float(VehicleSelectionView::getSizeY() / 2));

            // Calculate the starting point on the outside of the objective box.
            iXY objectiveOutlineSize(3, 3);

            //fXY o2vSlope(Math::unitDirection(b, a));

            //b.x += o2vSlope.x * float(objectiveOutlineSize.x);
            //b.y += o2vSlope.y * float(objectiveOutlineSize.y);

            //screen.drawLine(a, b, Color::white);

            iRect r(objectivePos - objectiveOutlineSize, objectivePos + objectiveOutlineSize);
            //bltBlendRect(screen, r);
            screen->fillRect(r, Color::white);

            //int xOffset = (strlen(WorldInputCmdProcessor::getSelectedObjectiveName()) * CHAR_XPIX) / 2;

            //screen.bltStringShadowed(r.min.x - xOffset, r.min.y - 15, WorldInputCmdProcessor::getSelectedObjectiveName(), Color::white, Color::black);

            iXY oos(objectiveOutlineSize);
            iXY cornerPos;

            if (v2oSlope.x > 0 && v2oSlope.y > 0) {
                cornerPos = iXY(max.x, max.y);
                r = iRect(cornerPos - oos, cornerPos + oos);
            }
            if (v2oSlope.x > 0 && v2oSlope.y <= 0) {
                cornerPos = iXY(max.x, min.y);
                r = iRect(cornerPos - oos, cornerPos + oos);
            }
            if (v2oSlope.x <= 0 && v2oSlope.y > 0) {
                cornerPos = iXY(min.x, max.y);
                r = iRect(cornerPos - oos, cornerPos + oos);
            }
            if (v2oSlope.x <= 0 && v2oSlope.y <= 0) {
                cornerPos = iXY(min.x, min.y);
                r = iRect(cornerPos - oos, cornerPos + oos);
            }

            screen->drawLine(cornerPos, b, Color::white);
            screen->fillRect(r, Color::white);

            //screen.bltLookup(r, Palette::darkGray256.getColorArray());
            //screen.drawButtonBorder(r, Color::white, Color::gray96);

            // Draw the name of the outpost.
        }}


    bltViewBackground(viewArea);

    int remaining_time = 0;
    int generation_time = 0;

    if ( obj->unit_generation_on_flag )
    {
        remaining_time = obj->unit_generation_timer.getTimeLeft();
        UnitProfile* profile = UnitProfileInterface::getUnitProfile(obj->unit_generation_type);
        generation_time = profile->regen_time;
    }

    if (vsvUnitGenOn)
    {
        sprintf(strBuf, "%s", getUnitName(vsvSelectedUnit));
        clientArea.bltString(   productionUnitPos.x, productionUnitPos.y,
                                strBuf, color);

        sprintf(strBuf, "%01d:%02d/%01d:%02d",
                        remaining_time / 60, remaining_time % 60,
                        generation_time / 60, generation_time % 60);

        clientArea.bltString(   timeRequiredPos.x, timeRequiredPos.y,
                                strBuf, color);
    }
    else
    {
        sprintf(strBuf, "power off");
        clientArea.bltString(   productionUnitPos.x, productionUnitPos.y,
                                strBuf, color);

        sprintf(strBuf, "power off");
        clientArea.bltString(   timeRequiredPos.x, timeRequiredPos.y,
                                strBuf, color);
    }

    int unitPerPlayer = GameConfig::game_maxunits / GameConfig::game_maxplayers;
    sprintf(strBuf, "%d/%d", int(UnitInterface::getUnitCount(PlayerInterface::getLocalPlayerIndex())), unitPerPlayer);
    clientArea.bltString(unitsBuiltPos.x, unitsBuiltPos.y, strBuf, color);

    drawUnitProfileInfo(clientArea, iXY(0, unitProfileDataY), highlightedUnitType);



    View::doDraw(viewArea, clientArea);

} // end VehicleSelectionView::doDraw

// drawUnitImage
//---------------------------------------------------------------------------
void VehicleSelectionView::drawUnitImage(Surface &dest, const iXY &pos, int unitType)
{
    unitImages.setFrame(unitType);
    unitImages.blt(dest, pos.x, pos.y);

} // end VehicleSelectionView::drawUnitImage

// getUnitName
//---------------------------------------------------------------------------
const char *VehicleSelectionView::getUnitName(int unitType)
{


    UnitProfile *p;


    p = UnitProfileInterface::getUnitProfile(unitType);



    if ( p )
    {
        return p->unitname.c_str();
    }
    return "Invalid unit type.";

 /*   if (unitType == _unit_type_valentine) {
        return "Manta";
    } else if (unitType == _unit_type_leopard) {
        return "Panther 1";
    } else if (unitType == _unit_type_abrams) {
        return "Titan";
    } else if (unitType == _unit_type_hammerhead) {
        return "Stinger";
    } else if (unitType == _unit_type_humvee) {
        // This is a hack to match other hacks.  When the scout is back in,
        // rename this to "Scout".
        return "SpahPanzer";
    } else if (unitType == _unit_type_lynx) {
        return "Bobcat";
    } else if (unitType == _unit_type_scorpion) {
        return "Wolf";
    } else if (unitType == _unit_type_spahpanzer) {
        return "Bear";
    } else if (unitType == _unit_type_m109) {
        return "Drake";
    } else if (unitType == _unit_type_archer) {
        return "Archer";
    } else if (unitType == _unit_type_null) {
        return "None";
    } else {
        return "Invalid unit type.";
    }
  */

} // end VehicleSelectionView::getUnitName

// getUnitRegenTime
//---------------------------------------------------------------------------
int VehicleSelectionView::getUnitRegenTime(unsigned short unitType)
{
    UnitProfile *profile;


    profile = UnitProfileInterface::getUnitProfile(unitType); //LOGGER.info("ststylesnum = %d", GameManager::ststylesnum);
    if ( profile )
        return (int) profile->regen_time;
    return 0;
 } // end VehicleSelectionView::getUnitRegenTime

// mouseMove
//---------------------------------------------------------------------------
void VehicleSelectionView::mouseMove(const iXY &prevPos, const iXY &newPos)
{
    GameTemplateView::mouseMove(prevPos, newPos);

    if ( highlightedUnitType >= 0 )
    {
        char strBuf[256];
        int rtime = getUnitRegenTime(highlightedUnitType);
        snprintf(strBuf, sizeof(strBuf)-1, "%s - Build Time: %01d:%02d",
                 getUnitName(highlightedUnitType), rtime / 60, rtime % 60);
        showStatus(strBuf);
    }
    else
    {
        showStatus("Select a unit for production");
    }

} // end VehicleSelectionView::mouseMove

// drawMiniProductionStatus
//---------------------------------------------------------------------------
void VehicleSelectionView::drawMiniProductionStatus(Surface &dest)
{
    char strBuf[256];
    char outpostNameBuf[256];
    char outpostUserNameBuf[256];
    char productionUnitBuf[256];
    char timeLeftBuf[256];

    iRect         gameViewRect;

    WorldViewInterface::getViewWindow(&gameViewRect);

    ObjectiveID objective_id;

    Objective* obj;
    ObjectiveID max_objective = ObjectiveInterface::getObjectiveCount();

    for ( objective_id = 0; objective_id < max_objective; ++objective_id)
    {
        obj = ObjectiveInterface::getObjective( objective_id );

        bool owned = obj->occupying_player && obj->occupying_player == PlayerInterface::getLocalPlayer();

        if ( ! displayOutpostNames && ( ! owned || ! displayMiniProductionStatus ) )
        {
            continue;
        }

        miniProductionRect.min = obj->area.getAbsRect(obj->location).min - gameViewRect.min;

        miniProductionRect.max.x = miniProductionRect.min.x + 158;  // 140
        miniProductionRect.max.y = miniProductionRect.min.y + (owned ? 50 : 20);
        if ( obj->occupying_player )
        {
            miniProductionRect.min.y-=16;
            int length = strlen( obj->occupying_player->getName().c_str() );
            if (length > 20)
            {
                strncpy(strBuf, obj->occupying_player->getName().c_str() , 17);
                strBuf[17] = 0; // fix runners
                sprintf(outpostUserNameBuf, "Owner: %s...", strBuf);
            }
            else
            {
                sprintf(outpostUserNameBuf, "Owner: %s", obj->occupying_player->getName().c_str() );
                //outpostUserNameBuf[26] = 0;
            }
        }
        iXY pos(miniProductionRect.min);
        pos.x += 4;
        pos.y += 4;

        // Make sure the name will fit reasonably in the area.
        int length = strlen( obj->name );
        if (length > 18) // was 10
        {
            strncpy(strBuf, (const char *) obj->name , 15);
            strBuf[15] = 0; // fix runners (was 7)
            sprintf(outpostNameBuf, "%s...",  strBuf);
        }
        else
        {
            sprintf(outpostNameBuf, "%s", (const char *) obj->name );
        }
        checkMiniProductionRect(outpostNameBuf);

        if ( owned )
        {
            if ( ! obj->unit_generation_on_flag )
            {
                // Objective is off.
                checkMiniProductionRect2(outpostUserNameBuf, outpostNameBuf);
                dest.bltLookup(miniProductionRect, Palette::darkGray256.getColorArray());
                dest.bltString(pos.x, pos.y, outpostUserNameBuf, Color::cyan);
                pos.y += 16;
                dest.bltString(pos.x, pos.y, outpostNameBuf, Color::white);
                pos.y += 16;
                dest.bltString(pos.x, pos.y, "Production Off", Color::white);
            }
            else
            {
                // Objective is on.

                sprintf(productionUnitBuf, "Production: %s", getUnitName(obj->unit_generation_type));
                //checkMiniProductionRect(productionUnitBuf);

                float tleft = obj->unit_generation_timer.getTimeLeft();
                sprintf(timeLeftBuf, "Time Left: %01d:%02d",
                        ((int)tleft) / 60,
                        ((int)tleft) % 60);
                checkMiniProductionRect3(outpostUserNameBuf, outpostNameBuf, productionUnitBuf);
                dest.bltLookup(miniProductionRect, Palette::darkGray256.getColorArray());

                dest.bltString(pos.x, pos.y, outpostUserNameBuf, Color::cyan);
                pos.x += unitImages.getWidth();
                pos.y += 16;
                dest.bltString(pos.x, pos.y, outpostNameBuf, Color::white);
                pos.y += 16;
                dest.bltString(pos.x, pos.y, productionUnitBuf, Color::white);
                pos.y += 16;
                dest.bltString(pos.x, pos.y, timeLeftBuf, Color::white);
                pos.y += 16;

                // Draw the current production unit image.
                drawUnitImage(dest, miniProductionRect.min + iXY(1,16), obj->unit_generation_type);
            }
        }
        else
        {
            checkMiniProductionRect2(outpostUserNameBuf, outpostNameBuf);
            dest.bltLookup(miniProductionRect, Palette::darkGray256.getColorArray());
            if ( obj->occupying_player)
            {
                dest.bltString(pos.x, pos.y, outpostUserNameBuf, Color::cyan);
                pos.y += 16;
            }
            dest.bltString(pos.x, pos.y, outpostNameBuf, Color::white);
        }
    }

} // end VehicleSelectionView::drawMiniProductionStatus

// checkMiniProductionRect
//---------------------------------------------------------------------------
// Purpose: Makes sure the rect is the size of the text inside.
//---------------------------------------------------------------------------
void VehicleSelectionView::checkMiniProductionRect(const std::string& string) // make 1 for 2 strings, 1 for 3 etc
{
    int length = Surface::getTextLength(string) + 16; //(was 54)

    if (length > miniProductionRect.getSizeX()) {
        miniProductionRect.max.x = miniProductionRect.min.x + length;
    }
} // end VehicleSelectionView::checkMiniProductionRect

void VehicleSelectionView::checkMiniProductionRect2(const std::string& string1, const std::string& string2)
{
    int length1 = Surface::getTextLength(string1) + 16; //(was 54)
    int length2 = Surface::getTextLength(string2) + 16;
    if (length1 >= length2) {
        if (length1 > miniProductionRect.getSizeX()) {
        miniProductionRect.max.x = miniProductionRect.min.x + length1;
        }
    } else {
        if (length2 > miniProductionRect.getSizeX()) {
        miniProductionRect.max.x = miniProductionRect.min.x + length2;
        }
    }
} // end VehicleSelectionView::checkMiniProductionRect

void VehicleSelectionView::checkMiniProductionRect3(const std::string& string1, const std::string& string2, const std::string& string3)
{
    int length1 = Surface::getTextLength(string1) + 16; //(was 54)
    int length2 = Surface::getTextLength(string2) + 52;
    int length3 = Surface::getTextLength(string3) + 52;

    if (length1 >= length2) {

        if (length1 >= length3) {

        if (length1 > miniProductionRect.getSizeX()) {
        miniProductionRect.max.x = miniProductionRect.min.x + length1;
        }

        } else {

        if (length3 > miniProductionRect.getSizeX()) {
        miniProductionRect.max.x = miniProductionRect.min.x + length3;
        }

        }


    } else {

        if (length2 >= length3) {

        if (length2 > miniProductionRect.getSizeX()) {
        miniProductionRect.max.x = miniProductionRect.min.x + length2;
        }

        } else {

        if (length3 > miniProductionRect.getSizeX()) {
        miniProductionRect.max.x = miniProductionRect.min.x + length3;
        }

        }


    }

} // end VehicleSelectionView::checkMiniProductionRect


// doActivate
//---------------------------------------------------------------------------
void VehicleSelectionView::doActivate()
{
    if (Desktop::getFocus() != this) {
        iXY pos;

        pos = MouseInterface::getMousePosition() - getSize() / 2;

        moveTo(pos);
        checkArea(iXY(screen->getWidth(),screen->getHeight()));
    }

    GameTemplateView::doActivate();

} // end VehicleSelectionView::doActivate

// getProfileData
//---------------------------------------------------------------------------
void VehicleSelectionView::getProfileData()
{
    for (unsigned int i = 0; i < UnitProfileInterface::getNumUnitTypes(); i++) {
        const UnitProfile *p = UnitProfileInterface::getUnitProfile(i);

        checkMaxValues(*p);
    }

} // end VehicleSelectionView::getProfileData

// checkMaxValues
//---------------------------------------------------------------------------
void VehicleSelectionView::checkMaxValues(const UnitProfile &profile)
{
    if (profile.hit_points > maxHitPoints) {
        maxHitPoints = profile.hit_points;
    }
    if (profile.attack_factor > maxAttackFactor) {
        maxAttackFactor = profile.attack_factor;
    }
    if (sqrt(profile.attack_range) > maxAttackRange) {
        maxAttackRange = int(sqrt(profile.attack_range));
    }
    if (sqrt(profile.defend_range) > maxDefendRange) {
        maxDefendRange = int(sqrt(profile.defend_range));
    }
    if (profile.speed_factor + profile.speed_rate > maxTotalSpeed) {
        maxTotalSpeed = profile.speed_factor + profile.speed_rate;
    }
    if (profile.reload_time > maxReloadTime) {
        maxReloadTime = profile.reload_time;
    }
    if (profile.regen_time > maxRegenTime) {
        maxRegenTime = profile.regen_time;
    }

} // end VehicleSelectionView::checkMaxValues

void VehicleSelectionView::drawUnitProfileInfo(Surface &dest, const iXY &pos, short int unitType)
{
    if (unitType == -1) {
        return;
    }


    const UnitProfile *profile;

    profile = UnitProfileInterface::getUnitProfile(unitType);





    iXY       loc       = pos;
    const int gapSpace  = 10;
    const int barOffset = 105;
    int       barLength = getClientRect().getSizeX() - barOffset;

    dest.bltStringShadowed(loc.x, loc.y, "Hit Points", Color::white, Color::black);
    drawBar(dest, iXY(loc.x + barOffset, loc.y), barLength, float(profile->hit_points) / float(maxHitPoints));
    loc.y += gapSpace;

    dest.bltStringShadowed(loc.x, loc.y, "Attack Power", Color::white, Color::black);
    drawBar(dest, iXY(loc.x + barOffset, loc.y), barLength, float(profile->attack_factor) / float(maxAttackFactor));
    loc.y += gapSpace;

    dest.bltStringShadowed(loc.x, loc.y, "Attack Range", Color::white, Color::black);
    drawBar(dest, iXY(loc.x + barOffset, loc.y), barLength, float(sqrt(profile->attack_range)) / float(maxAttackRange));
    loc.y += gapSpace;

    dest.bltStringShadowed(loc.x, loc.y, "Defend Range", Color::white, Color::black);
    drawBar(dest, iXY(loc.x + barOffset, loc.y), barLength, float(sqrt(profile->defend_range)) / float(maxDefendRange));
    loc.y += gapSpace;

    dest.bltStringShadowed(loc.x, loc.y, "Speed", Color::white, Color::black);
    drawBar(dest, iXY(loc.x + barOffset, loc.y), barLength, float(profile->speed_factor + profile->speed_rate) / float(maxTotalSpeed));
    loc.y += gapSpace;

    dest.bltStringShadowed(loc.x, loc.y, "Reload Time", Color::white, Color::black);
    drawBar(dest, iXY(loc.x + barOffset, loc.y), barLength, float(profile->reload_time) / float(maxReloadTime));
    loc.y += gapSpace;
}

void VehicleSelectionView::drawBar(Surface &dest, const iXY &pos, int length, float percent)
{
    iXY size(int(float(length) * percent), Surface::getFontHeight());

    dest.fillRect(iRect(pos.x, pos.y, pos.x + size.x, pos.y + size.y), Color::red);
}

// actionPerformed
//---------------------------------------------------------------------------
void VehicleSelectionView::actionPerformed(mMouseEvent me)
{
    if (me.getID() == mMouseEvent::MOUSE_EVENT_CLICKED) {


        if (me.getSource()==buttonAbandonS) {
            if (buttonAbandonS->getLabel() == "This") {
                buttonAbandonS->setLabel("This");
            } else {
                buttonAbandonS->setLabel("This");
            }
        Objective* obj = ObjectiveInterface::getObjective(CURRENT_SELECTED_OUTPOST_ID);
        if ( obj && obj->occupying_player == PlayerInterface::getLocalPlayer() ) {
        ObjectiveInterface::sendDisownObj(obj->id, 0, obj->occupying_player->getID());
        }
        }

        if (me.getSource()==buttonAbandonM) {
            if (buttonAbandonM->getLabel() == "All") {
                buttonAbandonM->setLabel("All");
            } else {
                buttonAbandonM->setLabel("All");
            }
        Objective* obj = ObjectiveInterface::getObjective(CURRENT_SELECTED_OUTPOST_ID);
        if ( obj && obj->occupying_player == PlayerInterface::getLocalPlayer() ) {
        ObjectiveInterface::sendDisownObj(obj->id, 1, obj->occupying_player->getID());
        }

        }


        if (me.getSource()==buttonPower) {
            if (buttonPower->getLabel() == "On") {
                buttonPower->setLabel("Off");
            } else {
                buttonPower->setLabel("On");
            }

            vsvUnitGenOn = !vsvUnitGenOn;
            changeMade   = true;
            sendOutpostStatus();
        }

        if (me.getSource()==buttonOk) {
            Desktop::setVisibilityNoDoAnything("VehicleSelectionView", false);
            sendOutpostStatus();
            changeMade = false;
        }
    }
} // end VehicleSelectionView::actionPerformed

// setPowerOn
//---------------------------------------------------------------------------
void VehicleSelectionView::setPowerOn()
{
    buttonPower->setLabel("On");

} // end VehicleSelectionView::setPowerOn

// setPowerOff
//---------------------------------------------------------------------------
void VehicleSelectionView::setPowerOff()
{
    buttonPower->setLabel("Off");

} // end VehicleSelectionView::setPowerOff

// doDeactivate
//---------------------------------------------------------------------------
void VehicleSelectionView::doDeactivate()
{
    GameTemplateView::doDeactivate();

    highlightedUnitType = -1;

} // end VehicleSelectionView::doDeactivate
