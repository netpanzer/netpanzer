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
#ifndef _SPRITESORTER_HPP
#define _SPRITESORTER_HPP

#include <vector>
#include "Classes/Sprite.hpp"

class SpriteSorter
{
protected:
    iRect world_window;
    std::vector<Sprite*> sprite_lists[ _MAX_HEIGHT_LEVELS ];
    unsigned long  list_counts[ _MAX_HEIGHT_LEVELS ];

    void sortLists( void );

protected:
    unsigned long max_sprite_stats[ _MAX_HEIGHT_LEVELS ];

public:

    SpriteSorter( );

    void initialize( );

    void reset( void );

    void reset( iRect &world_win );

    inline void setWorldWindow( iRect &world_win )
    {
        world_window = world_win;
    }

    inline iRect getWorldWindow( void )
    {
        return( world_window );
    }

    void addSprite( Sprite *sprite );

    inline bool cullSprite( Sprite &sprite )
    {
        return( !(sprite.isVisible( world_window )) );
    }

    void forceAddSprite( Sprite *sprite );

    void blitLists( Surface *render_surf );

    unsigned long getMaxSprites(unsigned long height_level) const;
};

extern SpriteSorter SPRITE_SORTER;

#endif // _SPRITESORTER_HPP
