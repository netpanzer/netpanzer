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


#include <algorithm>
#include <functional>
#include "Util/Exception.hpp"
#include "SpriteSorter.hpp"

SpriteSorter SPRITE_SORTER;

class SpriteCompare
            : public std::binary_function<const Sprite*, const Sprite*, bool>
{
public:
    bool operator()(const Sprite* sprite1, const Sprite* sprite2) const
    {
        long y1 = sprite1->world_pos.y;
        long y2 = sprite2->world_pos.y;

        //if (y1 > y2) return +1;
        if (y1 < y2) return true; else return false;

        // Tie based on the real comparison basis - impose an arbitrary
        // but consistent sort order to break the tie.

        //if (sprite1 > sprite2) return +1;
        if (sprite1 < sprite2) return true; else return false;

        // We should never get here, unless the same sprite is
        // inserted in the list twice
        throw Exception("sprite_key called to compare the same sprite against itself!");
    }
};

SpriteSorter::SpriteSorter( )
{
    initialize();
}

void SpriteSorter::initialize( )
{
    unsigned long i;

    for ( i = 0; i < 3; i++ ) {
        sprite_lists[i].resize(50);
    }

    sprite_lists[3].resize(500);
    sprite_lists[4].resize(500);
    sprite_lists[5].resize(500);

    for ( i = 6; i < _MAX_HEIGHT_LEVELS; i++ ) {
        sprite_lists[i].resize(50);
    }

    for ( i = 0; i < _MAX_HEIGHT_LEVELS; i++ ) {
        list_counts[i] = 0;
        max_sprite_stats[i] = 0;
    }
}

void SpriteSorter::reset( void )
{
    unsigned long i;

    for ( i = 0; i < _MAX_HEIGHT_LEVELS; i++ ) {
        list_counts[ i ] = 0;
    }

}

void SpriteSorter::reset( iRect &world_win )
{
    world_window = world_win;
    reset();
}

void SpriteSorter::addSprite(Sprite *sprite)
{
    if ( sprite->isVisible( world_window ) ) {
        forceAddSprite(sprite);
    }
}

void SpriteSorter::forceAddSprite(Sprite *sprite)
{
    unsigned char height;

    height = sprite->getSpriteHeight();

    assert( height < _MAX_HEIGHT_LEVELS );

    if(sprite_lists[height].size() <= list_counts[height])
        sprite_lists[height].resize(1 + list_counts[height]);
    sprite_lists[height] [list_counts[height]] = sprite;

    list_counts[ height ]++;
}

void SpriteSorter::sortLists()
{
    unsigned long i;

    for ( i = 0; i < _MAX_HEIGHT_LEVELS; i++ ) {
        sort(sprite_lists[i].begin(), sprite_lists[i].end(),
             SpriteCompare());

        //sprite_lists[ i ].sort( list_counts[ i ], sprite_key );
    }
}

void SpriteSorter::blitLists( Surface *render_surf )
{
    Sprite *sprite;
    unsigned long list_index, sprite_index;

    //sortLists();

    for ( list_index = 0; list_index < _MAX_HEIGHT_LEVELS; list_index++ ) {
        for ( sprite_index = 0; sprite_index < list_counts[ list_index]; sprite_index++ ) {
            sprite = (Sprite *) sprite_lists[ list_index ][ sprite_index ];
            sprite->blitAll( render_surf, world_window );
        }

        if ( list_counts[ list_index ] > max_sprite_stats[ list_index ] ) {
            max_sprite_stats[ list_index ] = list_counts[ list_index ];
        }
    }
}

unsigned long SpriteSorter::getMaxSprites(unsigned long height_level) const
{
    return( max_sprite_stats[ height_level ] );
}

