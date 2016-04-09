/*
Copyright (C) 2004 by Matthias Braun <matze@braunis.de>

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
#ifndef __NETPACKETDEBUGGER_HPP__
#define __NETPACKETDEBUGGER_HPP__

#include <iostream>

class NetMessage;
class NetPacket;
class UMesgAICommand;

class NetPacketDebugger
{
public:
    static void logPacket(const char* domain, const NetPacket* packet);
    static void logMessage(const char* domain, const NetMessage* message);

private:
    static void logTerminalMessage(std::ostream& stream, const NetMessage* message);
    static void logAICommand(std::ostream& str, const UMesgAICommand& command);
    static void logMultiMessage(std::ostream& str, const NetMessage* message);
    static void logUnitMessage(std::ostream& log, const NetMessage* message);
    static void logUnitOpcodeMessage(std::ostream& log, const NetMessage* message);
    static void logConnectMessage(std::ostream& log, const NetMessage* message);
};

#endif

