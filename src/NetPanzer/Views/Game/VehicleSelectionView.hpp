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
#ifndef __VehicleSelectionView_hpp__
#define __VehicleSelectionView_hpp__

#include "Core/CoreTypes.hpp"
#include "2D/Surface.hpp"
#include "Views/Components/View.hpp"
#include "GameTemplateView.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "Views/Components/Component.hpp"
#include "Views/Components/Button.hpp"
#include "Interfaces/GameManager.hpp"


// The following may be used some day.
/*
class NETPANZER_UNIT
{
public:
  Surface hull;
  Surface turret;
  enum { NUM_CHARS = 40 };

  char length[NUM_CHARS];
  char width[NUM_CHARS];
  char height[NUM_CHARS];
  char weight[NUM_CHARS];
  char engine[NUM_CHARS];
  char speed[NUM_CHARS];
  char radiusOfAction[NUM_CHARS];
  char groundPressure[NUM_CHARS];
  char armament[NUM_CHARS];
  char crew[NUM_CHARS];
  char notes[NUM_CHARS];

}; // end UNIT
*/

extern int vsvSelectedUnit;     // Vehicle Selection View Selected Unit
extern int vsvTempSelectedUnit; // Vehicle Selection View Temp Selected Unit

void activateVehicleSelectionView( ObjectiveID outpost_id );
void toggleDisplayOutpostNames( void );

//--------------------------------------------------------------------------
class VehicleSelectionView : public GameTemplateView
{
private:
    friend class UnitSelectionButton;
    int highlightedUnitType;

    static Button * buttonAbandonS;
    static Button * buttonAbandonM;
    static Button * buttonPower;
    static Button * buttonOk;

    int maxHitPoints;
    int maxAttackFactor;
    int maxAttackRange;
    int maxDefendRange;
    int maxTotalSpeed;
    int maxReloadTime;
    int maxRegenTime;
    int unitProfileDataY;

    iXY buttonSize;

    int  getUnitRegenTime(unsigned short unitType);
    void checkMaxValues(const UnitProfile &profile);
    void getProfileData();
    void drawBar(Surface &dest, const iXY &pos, int length, float percent);
    void drawUnitProfileInfo(Surface &dest, const iXY &pos, short int unitType);

    iXY            productionUnitPos;
    iXY            timeRequiredPos;
    iXY            timeRemainingPos;
    iXY            unitsBuiltPos;

    static Surface unitImages;

    static void checkMiniProductionRect(const std::string& string);
    static void checkMiniProductionRect2(const std::string& string1, const std::string& string2);
    static void checkMiniProductionRect3(const std::string& string1, const std::string& string2, const std::string& string3);



public:
    VehicleSelectionView();
    virtual ~VehicleSelectionView()
    {}

    virtual void        doActivate();
    virtual void        doDraw(Surface &windowArea, Surface &clientArea);
    virtual void        mouseMove(const iXY &prevPos, const iXY &newPos);
    static  const char *getUnitName(int unitType);
    static  void        drawMiniProductionStatus(Surface &dest);
    static  void        drawUnitImage(Surface &dest, const iXY &pos, int unitType);
    static  bool        displayMiniProductionStatus;
    static  bool        displayOutpostNames;
    static  void        setPowerOn();
    static  void        setPowerOff();
    virtual void        doDeactivate();

    static iRect   miniProductionRect;

    virtual void actionPerformed(mMouseEvent me);

}; // end VehicleSelectionView

#endif // end __VehicleSelectionView_hpp__
