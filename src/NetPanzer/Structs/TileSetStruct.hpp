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
#ifndef _TILESETSTRUCT_HPP
#define _TILESETSTRUCT_HPP

#define TILE_XPIX 32
#define TILE_YPIX 32

/*
.tls structure:
Header
Data	-> unsigned char
Graphics
*/

class TILE_DBASE_HEADER
{
public:
    unsigned char	netp_id_header[64];
    unsigned short	version;
    unsigned short	x_pix;
    unsigned short	y_pix;
    unsigned short	tile_count;
    unsigned char	palette[768];

}
; // end TILE_DBASE_HEADER

class TILE_HEADER
{
public:
    char	attrib;
    char	move_value;
    char	avg_color;

}
; // end TILE_HEADER

#endif  // ** _TILESETSTRUCT_HPP
