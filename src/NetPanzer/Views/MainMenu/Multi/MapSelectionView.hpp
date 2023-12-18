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
#ifndef __MapSelectionView_hpp__
#define __MapSelectionView_hpp__

#include <vector>
#include <string.h>

#include "Views/MainMenu/RMouseHackView.hpp"
#include "2D/Surface.hpp"
#include "Views/Components/Choice.hpp"

//---------------------------------------------------------------------------
class MapInfo
{
public:
    Surface thumbnail;
    std::string name;
    std::string description;
    iXY     cells;
    int     objectiveCount;

    MapInfo()
    {
    }
}; // end MapInfo

//---------------------------------------------------------------------------
class MapSelectionView : public RMouseHackView
{
private:
    enum { BORDER_SPACE =   4 };
    enum { MAP_SIZE     = 100 };
    int  loadMaps();
    void drawCurMapInfo(Surface &dest, const iXY &pos);



public:
    MapSelectionView();
    ~MapSelectionView();

    virtual void doDraw(Surface &windowArea, Surface &clientArea);

    static std::vector<MapInfo*> mapList;
    static int curMap;

    void init();




}; // end MapSelectionView

#endif // end __MapSelectionView_hpp__
