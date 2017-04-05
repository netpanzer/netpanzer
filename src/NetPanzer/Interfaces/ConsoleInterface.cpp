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


#include "Interfaces/ConsoleInterface.hpp"

#include "cstring"

#include <time.h>
#include "Interfaces/GameConfig.hpp"
#include "Util/Log.hpp"
#include "Console.hpp"
#include "Resources/ResourceManager.hpp"

#define CommandMax 13

std::string CommandList[CommandMax] = {
    "/server listcommands", "/server adminlogin", "/server unitspawnlist", "/server unitprofiles",
    "/server listprofiles", "/server kick", "/server baselimit", "/server gamepass",
    "/server map ", "/server autokick", "/server say", "/server listplayers", "/server flagtimer"};

bool ConsoleInterface::stdout_pipe;

long ConsoleInterface::console_size;
ConsoleLineArray ConsoleInterface::line_list;

iXY ConsoleInterface::surface_size;
iRect    ConsoleInterface::bounds;
iXY ConsoleInterface::line_offset;
long     ConsoleInterface::vertical_spacing;
long     ConsoleInterface::horizontal_spacing;
long     ConsoleInterface::max_char_per_line;

long ConsoleInterface::line_index;

bool ConsoleInterface::input_string_active;

int  ConsoleInterface::maxCharCount;
char ConsoleInterface::inputString[256];
char ConsoleInterface::inputPrompt[256];
int  ConsoleInterface::cursorPos;
int ConsoleInterface::commandPos;
void ConsoleInterface::initialize( long size )
{
    assert( size > 0 );

    console_size = size;
    line_list.initialize( size );

    line_index = console_size - 1;

    surface_size = iXY( 800, 600 );
    bounds = iRect( 5, 5+12, 800 - 5, 600 - (5+12) );

    max_char_per_line = (bounds.max.x - bounds.min.x) / 8;

    vertical_spacing = 2;

    line_offset.x = 0;
    line_offset.y = (14 + vertical_spacing);

    input_string_active = false;

    long line_loop;

    for ( line_loop = 0; line_loop < console_size; line_loop++ )
    {
        line_list[ line_loop ].color = Color::white;
        line_list[ line_loop ].str[0] = 0;
        line_list[ line_loop ].life_timer.changePeriod( 30 );
        line_list[ line_loop ].visible = false;
    }
    commandPos = 0;
    stdout_pipe = false;
}

void ConsoleInterface::setToSurfaceSize( iXY pix )
{
    surface_size = pix;

    bounds.min.x = 5;
    bounds.min.y = 5;
    bounds.max = pix - iXY(5, 5);

    int CHAR_XPIX = 8; // XXX hardcoded
    max_char_per_line = (bounds.max.x - bounds.min.x) / CHAR_XPIX;
}

void ConsoleInterface::setStdoutPipe( bool on_off )
{
    stdout_pipe = on_off;
}

void ConsoleInterface::update( Surface &surface )
{
    update_overlap( surface );
}

void ConsoleInterface::postMessage(PIX msgcolor, bool hasFlag, FlagID flag, const char *format, ...)
{
    char temp_str[256];
    char *temp_str_ptr;
    long temp_str_length;
    va_list vap;

    va_start( vap, format );
    vsnprintf( temp_str, 256, format, vap );
    va_end( vap );

    LOGGER.debug("C: %s", temp_str);

    if (stdout_pipe && Console::server) {
        *Console::server << temp_str << std::endl;
    }

    temp_str_ptr = temp_str;
    temp_str_length = (long) strlen(temp_str);

    if( temp_str_length > max_char_per_line ) {
        long partion_count = temp_str_length / max_char_per_line;

        for( int i = 0; i < partion_count; i++ ) {
            if (line_index == 0)
                line_index = console_size - 1;
            else
                line_index = (line_index - 1) % console_size;

            memset(line_list[ line_index ].str, 0, 256);
            strncpy( line_list[ line_index ].str, temp_str_ptr, max_char_per_line);
            line_list[ line_index ].str[ max_char_per_line ] = 0;

            line_list[ line_index ].color = msgcolor;
            line_list[ line_index ].visible = true;
            line_list[ line_index ].life_timer.reset();
            line_list[ line_index ].hasFlag = hasFlag;
            line_list[ line_index ].flag = flag;

            temp_str_ptr = temp_str_ptr + max_char_per_line;
        }

    }

    if (line_index == 0)
        line_index = console_size - 1;
    else
        line_index = (line_index - 1) % console_size;

    strcpy( line_list[ line_index ].str, temp_str_ptr );

    line_list[ line_index ].color = msgcolor;
    line_list[ line_index ].visible = true;
    line_list[ line_index ].hasFlag = hasFlag;
    line_list[ line_index ].flag = flag;
    line_list[ line_index ].life_timer.reset();
}

void ConsoleInterface::update_overlap( Surface &surface )
{
    iXY current_line;
    long  index;
    short visible_count;

    index = line_index;

    visible_count = 0;

    do {
        if ( line_list[ index ].life_timer.count() ) {
            line_list[ index ].visible = 0;
        }

        if ( line_list[ index ].visible )
            visible_count++;

        index = (index + 1) % console_size;

    } while( index != line_index );

    if( input_string_active == true ) {
        current_line.y = bounds.min.y + (line_offset.y * visible_count ) + (line_offset.y * 2);
        current_line.x = bounds.min.x + line_offset.x;
    } else {
        current_line.y = bounds.min.y + (line_offset.y * visible_count );
        current_line.x = bounds.min.x + line_offset.x;
    }

    int flagextrax;
    Surface * flag = 0;
    do
    {
        if ( line_list[ index ].visible )
        {
            if ( line_list[ index ].hasFlag )
            {
                flag = ResourceManager::getFlag(line_list[index].flag);
                flagextrax = flag->getWidth()+2;
                flag->blt(surface, current_line.x, current_line.y);
            }
            else
            {
                flagextrax = 0;
            }
            // XXX some values by hand
            surface.bltStringShadowed(current_line.x + flagextrax, current_line.y+4, line_list[ index ].str, line_list[ index ].color, Color::black );

            current_line.y = current_line.y - line_offset.y;
        }

        index = (index + 1) % console_size;

    } while( index != line_index );


    if( input_string_active == true ) {
        iXY input_offset;
        unsigned long max_char_space;
        unsigned long input_string_length;
        char *string_ptr;

        current_line.y = current_line.y - line_offset.y;

        surface.bltStringShadowed(current_line.x, current_line.y, inputPrompt, Color::white,
                Color::black );

        int CHAR_XPIX = 8; // XXX hardcoded
        input_offset.x = current_line.x + ( (long) strlen( inputPrompt ) ) * CHAR_XPIX;
        input_offset.y = current_line.y;

        max_char_space = (bounds.max.x - input_offset.x ) / CHAR_XPIX;
        input_string_length = strlen(inputString);

        if( input_string_length > max_char_space ) {
            string_ptr = inputString + (input_string_length - (max_char_space - 1) );
        } else {
            string_ptr = inputString;
        }

        surface.bltStringShadowed(input_offset.x, input_offset.y, string_ptr , Color::white,
                Color::black );

        surface.bltStringShadowed(  input_offset.x + cursorPos * CHAR_XPIX,
                                    input_offset.y,
                                    "_", Color::white, Color::black );
    }
}

void ConsoleInterface::setInputStringStatus( bool on_off )
{
    input_string_active = on_off;
}

void ConsoleInterface::resetInputString( const char *prompt )
{
    cursorPos = 0;
    inputString[0] = 0;
    maxCharCount = 256;
    strcpy( inputPrompt, prompt );
}

void ConsoleInterface::getInputString( char *str )
{
    strcpy( str, inputString );
}

void ConsoleInterface::addChar(int newChar)
{
    // Check if the character should be excluded.
    // Add the character.
    int length = strlen(inputString) + 1;

    if ( cursorPos != length )
    {
        memmove(&inputString[cursorPos+1], &inputString[cursorPos], length-cursorPos);
        ++length;
    }

    inputString[cursorPos] = newChar;
    inputString[length] = '\0';

    if (cursorPos < maxCharCount - 1) {
        cursorPos++;
    }
} // addChar

void ConsoleInterface::addExtendedChar(int newExtendedChar)
{
    // Process the extendedChar accordingly.
    switch (newExtendedChar) {
    case SDLK_RETURN: {}
        break;

    case SDLK_HOME: {
            cursorPos = 0;
        }
        break;

    case SDLK_LEFT: {
            if(--cursorPos < 0) {
                cursorPos = 0;
            }
        }
        break;

    case SDLK_RIGHT: {
            int length = strlen(inputString);
            if(++cursorPos > length) {
                if (cursorPos > maxCharCount) {
                    cursorPos = maxCharCount - 1;
                } else {
                    cursorPos = length;
                }
            }
        }
        break;

    case SDLK_END: {
            cursorPos = strlen(inputString);

            if (cursorPos > maxCharCount) {
                cursorPos = maxCharCount - 1;
            }
        }
        break;

    case SDLK_INSERT: {}
        break;

    case SDLK_DELETE: {
            if (cursorPos == (int) strlen(inputString + cursorPos)) {
                break;
            }

            memcpy(inputString + cursorPos, inputString + cursorPos + 1, strlen(inputString + cursorPos + 1) + 1);
        }
        break;

    case SDLK_BACKSPACE: {
            if (cursorPos >= 1) {
                int byteCount = strlen(inputString + cursorPos);

                // Only do this if we are not at the end of the string.
                if (byteCount > 0) {
                    memcpy(inputString + cursorPos - 1, inputString + cursorPos, byteCount);
                }

                cursorPos--;
                inputString[cursorPos + byteCount] = '\0';
            }
        }
        break;

    case SDLK_TAB: {
            cursorPos = 0;
            strcpy(inputString, CommandList[commandPos].c_str());
            commandPos++;
            if (commandPos >= CommandMax) commandPos = 0;
            cursorPos = strlen(inputString);
        }
        break;

    default: break;
    }

} // addExtendedChar
