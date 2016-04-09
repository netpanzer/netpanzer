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


#include "BoundBox.hpp"

bool BoundBox::bounds(const iXY& center, const iXY& test ) const
{
    iXY new_max( center + max );
    iXY new_min( center + min );

    return( test.x >= new_min.x && test.x < new_max.x &&
            test.y >= new_min.y && test.y < new_max.y
          );
}

void BoundBox::setBounds(const iXY &nMin, const iXY &nMax )
{
    min = iXY(nMin);
    max = iXY(nMax);
    assert( min.x <= 0 && min.y <= 0 && max.x >= 0 && max.y >= 0 );
}
