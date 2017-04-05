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
#ifndef __NETWORK_GLOBALS_HPP__
#define __NETWORK_GLOBALS_HPP__

#define NETPANZER_DEFAULT_PORT_TCP     3030
#define NETPANZER_PROTOCOL_VERSION     1128
#define MASTERSERVER_PORT             28900
#define AUTHSERVER_PORT               28888

const char * getNetpanzerProtocolMessage(const int protocol);

#endif
