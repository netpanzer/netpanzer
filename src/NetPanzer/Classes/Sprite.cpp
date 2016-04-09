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

#include "Classes/Sprite.hpp"
#include "2D/Palette.hpp"

Sprite::Sprite( )
{
    height = 0;
    visible = true;
    next = 0;

}

SpritePacked::SpritePacked()
        : Sprite()
{
    colorTable = 0;
    drawMode   = SOLID;
}

SpritePacked::SpritePacked( PackedSurface &source )
        : Sprite()
{
    setTo( source );
    colorTable = 0;
    drawMode   = SOLID;
}

bool SpritePacked::isVisible(const iRect &world_win ) const
{
    long min_x;
    long min_y;
    long max_x;
    long max_y;
    long pix_x;
    long pix_y;

    pix_x = pix.x >> 1;
    pix_y = pix.y >> 1;

    min_x = world_pos.x - pix_x;
    min_y = world_pos.y - pix_y;

    max_x = world_pos.x + pix_x;
    max_y = world_pos.y + pix_y;

    if ( (min_y >= world_win.max.y) || (max_y <= world_win.min.y)	||
            (min_x >= world_win.max.x) || (max_x <= world_win.min.x)
       )
        return( false );

    return( true );

    /*
     iXY temp_pix = pix;
     temp_pix.x = temp_pix.x/2;
     temp_pix.y = temp_pix.y/2;
        
     if ( world_win.clip( iRect( world_pos - temp_pix, world_pos + temp_pix) ) )  
      return( false );

     return( true );  
     */
}

void SpritePacked::blit( Surface *surface, const iRect &world_win )
{
    iXY blit_offset;

    blit_offset = (world_pos + attach_offset) - world_win.min;

    if (drawMode == BLEND) {
        bltBlend(*surface, blit_offset.x, blit_offset.y, *colorTable);

    } else if (drawMode == SOLID) {
        blt(*surface, blit_offset);
    } else {
        assert(false);
    }
}
