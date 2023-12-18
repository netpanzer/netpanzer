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
#ifndef __SERVERCONSOLE_HPP__
#define __SERVERCONSOLE_HPP__

#include <string>
#include <SDL2/SDL_thread.h>
#include "DedicatedGameManager.hpp"

class DedicatedGameManager;

class ServerConsole
{
public:
    ServerConsole(DedicatedGameManager* manager);
    ~ServerConsole();

    void startThread();
    void executeCommand(const std::string& command);
    
private:
    static int _run(ServerConsole* _this);
    void run();
    
    void commandQuit();

    volatile bool running;
    SDL_Thread* thread;
    DedicatedGameManager* manager;
};

#endif

