/*
Copyright (C) 2003 Matthias Braun <matze@braunis.de>

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

#include "DummySound.hpp"

#include "Util/Exception.hpp"
#include "Util/Log.hpp"

DummySound::DummySound() {}

DummySound::~DummySound() {}

void DummySound::playSound(const char*) {}

void DummySound::playAmbientSound(const char*, long) {}

void DummySound::playMusic(const char*) {}

void DummySound::stopMusic() {}

void DummySound::setSoundVolume(unsigned int) {}

void DummySound::setMusicVolume(unsigned int) {}
