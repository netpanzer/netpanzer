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


#include "BresenhamLine.hpp"

void BresenhamLine::set_path( iXY &start, iXY &end )
{

    finished = 0;

    x = start.x;
    y = start.y;

    DeltaX = end.x - x;
    DeltaY = end.y - y;

    if ( DeltaY > 0 ) {
        if ( DeltaX > 0 ) {
            if ( DeltaX > DeltaY ) {
                Octant = 0;
                IniOctant_0_3();
            } else {
                Octant = 1;
                IniOctant_1_2();
            }
            XDirection = 1;
        } // ** Delta > 0
        else {
            DeltaX = -DeltaX;
            if ( DeltaX > DeltaY ) {
                Octant = 0;
                IniOctant_0_3();
            } else {
                Octant = 1;
                IniOctant_1_2();
            }
            XDirection = -1;
        }
    } // ** DeltaY > 0
    else {
        DeltaY = -DeltaY;

        if ( DeltaX > 0 ) {
            if ( DeltaX > DeltaY ) {
                Octant = 3;
                IniOctant_4_7();
            } else {
                Octant = 4;
                IniOctant_5_6( );
            }
            XDirection = 1;
        } // ** DeltaX > 0 && DeltaY < 0
        else {
            DeltaX = -DeltaX;
            if ( DeltaX > DeltaY ) {
                Octant = 3;
                IniOctant_4_7();
            } else {
                Octant = 4;
                IniOctant_5_6( );
            }
            XDirection = -1;
        }

    } // ** DeltaY < 0
}


short BresenhamLine::increment(  iXY *update, short times )
{

    while ( (times != 0) && (finished == false) ) {
        switch(Octant) {
        case 0 : if ( !finished)
                finished = Octant_0_3(); break;
        case 1 : if ( !finished)
                finished = Octant_1_2(); break;
        case 3 : if ( !finished)
                finished = Octant_4_7(); break;
        case 4 : if ( !finished)
                finished = Octant_5_6(); break;
        }

        times--;
    }

    update->x = x;
    update->y = y;

    return( finished );

}

void BresenhamLine::IniOctant_0_3( void )
{
    Deltax2 = DeltaY * 2;
    DeltaAx2minusDeltaBx2 = Deltax2 - (DeltaX * 2);
    ErrorTerm = Deltax2 - DeltaX;
}

void BresenhamLine::IniOctant_1_2( void )
{
    Deltax2 = DeltaX * 2;
    DeltaAx2minusDeltaBx2 = Deltax2 - (DeltaY * 2);
    ErrorTerm = Deltax2 - DeltaY;
}

void BresenhamLine::IniOctant_4_7( void )
{
    Deltax2 = DeltaY * 2;
    DeltaAx2minusDeltaBx2 = Deltax2 - (DeltaX * 2);
    ErrorTerm = Deltax2 - DeltaX;
}

void BresenhamLine::IniOctant_5_6( void )
{
    Deltax2 = DeltaX * 2;
    DeltaAx2minusDeltaBx2 = Deltax2 - (DeltaY * 2);
    ErrorTerm = Deltax2 - DeltaY;
}

//**************************************************************************************
//######################################################################################

short BresenhamLine::Octant_0_3( void )
{

    DeltaX--;

    if ( ( (DeltaX) >= 0 ) ) {
        if ( ErrorTerm >= 0 ) // ** Advance Y cordinate
        {
            y++;
            ErrorTerm += DeltaAx2minusDeltaBx2;
        } else
            ErrorTerm += Deltax2;

        x += XDirection;
    } // ** while


    if (DeltaX <= 0)
        return( 1 );
    else
        return( 0 );
}

//**************************************************************************************
//######################################################################################

short BresenhamLine::Octant_1_2( void )
{

    DeltaY--;

    if ( (DeltaY) >= 0   ) {
        if ( ErrorTerm >= 0 ) // ** Advance Y cordinate
        {
            x+= XDirection;
            ErrorTerm += DeltaAx2minusDeltaBx2;
        } else
            ErrorTerm += Deltax2;

        y++;
    } // ** while

    if (DeltaY <= 0)
        return( 1 );
    else
        return( 0 );


}


//**************************************************************************************
//######################################################################################

short BresenhamLine::Octant_4_7( void )
{
    DeltaX--;

    if ( ( (DeltaX) >= 0 ) ) {
        if ( ErrorTerm >= 0 ) // ** Advance Y cordinate
        {
            y--;
            ErrorTerm += DeltaAx2minusDeltaBx2;
        } else
            ErrorTerm += Deltax2;

        x += XDirection;
    } // ** while

    if (DeltaX <= 0)
        return( 1 );
    else
        return( 0 );
}


//**************************************************************************************
//######################################################################################

short BresenhamLine::Octant_5_6( void )
{
    DeltaY--;

    if ( ( (DeltaY) >= 0 ) ) {
        if ( ErrorTerm >= 0 ) // ** Advance Y cordinate
        {
            x+= XDirection;
            ErrorTerm += DeltaAx2minusDeltaBx2;
        } else
            ErrorTerm += Deltax2;

        y--;
    } // ** while

    if (DeltaY <= 0)
        return( 1 );
    else
        return( 0 );

}
