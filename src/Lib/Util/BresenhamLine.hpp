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
#ifndef _BRESENHAMLINE_HPP
#define _BRESENHAMLINE_HPP

#include "Types/iXY.hpp"

class BresenhamLine
{
protected:
    long  x;
    long  y;

    long  DeltaX;
    long  DeltaY;

    long  Deltax2;
    long  DeltaAx2minusDeltaBx2;
    long  ErrorTerm;
    short XDirection;
    short Octant;

    short finished;

    void IniOctant_0_3();
    void IniOctant_1_2();
    void IniOctant_4_7();
    void IniOctant_5_6();

    short Octant_0_3();
    short Octant_1_2();
    short Octant_4_7();
    short Octant_5_6();

public:
    void set_path(iXY &start, iXY &end);

    short increment(iXY *update, short times);
};


#endif // ** _BRESENHAMLINE_HPP
