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

#include "Core/CoreTypes.hpp"
#include "Classes/SelectionBoxSprite.hpp"
#include "2D/Palette.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Resources/ResourceManager.hpp"

bool SelectionBoxSprite::isVisible(const iRect &world_win) const
{
    if (
        (world_win.contains( world_pos + selection_area.min ) && (visible == true ) ) ||
        (world_win.contains( world_pos + selection_area.max ) && (visible == true ) )
    )
        return( true );

    return( false );
}


void SelectionBoxSprite::blit( Surface *surface, const iRect &world_win )
{
    iXY min_abs, max_abs;

    if ( box_state == false )
        return;

    min_abs = (world_pos + selection_area.min) - world_win.min;
    max_abs = (world_pos + selection_area.max) - world_win.min;

    if( (min_abs.x >= 0 ) ) {
        surface->drawVLine(min_abs.x, min_abs.y, max_abs.y, box_color);
    }

    if( ((unsigned int)max_abs.x < surface->getWidth())  ) {
        surface->drawVLine(max_abs.x, min_abs.y, max_abs.y, box_color);
    }

    if( (min_abs.y >= 0 ) ) {
        surface->drawHLine(min_abs.x, min_abs.y, max_abs.x, box_color);
    }

    if ( ((unsigned int)max_abs.y < surface->getHeight()) ) {
        surface->drawHLine(min_abs.x, max_abs.y, max_abs.x+1, box_color);
    }

}

UnitSelectionBox::UnitSelectionBox( )
{
    allied_visiblity_state = false;
    allie_state = false;
    flag_visibility_state = false;
    unit_flag = 0;
}


void UnitSelectionBox::blit( Surface *surface, const iRect &world_win )
{
    iXY min_abs, max_abs;

    min_abs = (world_pos + selection_area.min) - world_win.min;
    max_abs = (world_pos + selection_area.max) - world_win.min;

    if ( box_state == true )
    {

        // Modified the vehicle selection box and moved the hitpoints outside,
        // the box status check, because I may want the hitpoints drawn all the time.
        PIX selectionBoxColor = gameconfig->getVehicleSelectionBoxColor();

        assert(max_hit_points > 0);

        // Draw the selection box.
        if (GameConfig::interface_unitselectionmode == _unit_selection_box_draw_mode_rect)
        {
            // Draw the rectangle selection box.
            surface->drawRect(iRect(min_abs, max_abs), selectionBoxColor);
        }
        else // if (gameconfig->unitselectionmode == _unit_selection_box_draw_mode_rect_edges)
        {
            surface->drawBoxCorners(
                    iRect(min_abs.x, min_abs.y, max_abs.x, max_abs.y),
                    7, selectionBoxColor);
        }

    } // ** box_state == true

    // Draw the unit hitpoints.
    if ( GameConfig::interface_show_health || (box_state == true) )
    {
        // Draw a color coded hit bar.
        Uint8 hitBarColor;

        float hitPointPercent = float(hit_points) / float(max_hit_points);

        if (hitPointPercent < 0.3f)
        {
            hitBarColor = Color::red;
        }
        else if (hitPointPercent < 0.6f)
        {
            hitBarColor = Color::yellow;
        }
        else
        {
            hitBarColor = Color::green;
        }

        int hit_bar_size = int ( (float) (max_abs.x - min_abs.x - 5) * ( hitPointPercent ) );

        iRect r(min_abs.x + 1, max_abs.y - 5, max_abs.x - 1, max_abs.y - 1);

        surface->bltLookup(r, Palette::darkGray256.getColorArray());

        r = iRect(min_abs.x + 2, max_abs.y - 4, min_abs.x + 2 + hit_bar_size, max_abs.y - 3);

        surface->drawRect(r, hitBarColor);

        // Solid method.
        //surface->drawHLine(min_abs.x+1, max_abs.y-1, min_abs.x + hit_bar_size, hit_bar_color);
        //surface->drawHLine(min_abs.x+1, max_abs.y-2, min_abs.x + hit_bar_size, hit_bar_color);
        //surface->drawHLine(min_abs.x, max_abs.y-3, max_abs.x+1, box_color);
    }

    if ( GameConfig::interface_show_flags == true  && unit_flag )
    {
        //unit_flag.blt( *surface, iXY( min_abs.x, min_abs.y - unit_flag.getPix().y ) );
        //surface->bltString(min_abs.x + 2, min_abs.y - 6, "Panther1", Color::white);
        unit_flag->blt( *surface, min_abs.x, min_abs.y - FLAG_HEIGHT - 1 );
    }

    if ( GameConfig::interface_show_names == true )
    {
        if ( playerName.length() > 0 )
        {   // XXX dirty trick, I don't center the text, just "by hand"
            surface->bltString( min_abs.x + FLAG_WIDTH  + 2,
                                min_abs.y - FLAG_HEIGHT + 2,
                                playerName.c_str(), Color::gray224);
        }
    }

    /*
    if ( (flag_visibility_state == true) && (allie_state == true) )
     { 
      allie_flag.blt ( *surface, iXY( max_abs.x - allie_flag.getPix().x , min_abs.y - allie_flag.getPix().y ) );
     }
    */


}
