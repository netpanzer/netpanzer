/*
Copyright (C) 2024 NetPanzer (https://github.com/netpanzer/), Andy Alt, Et al.

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

#ifndef TEST_LIB

#include "package.hpp"

#ifndef PACKAGE_VERSION
#include "config.h"
#endif

const std::string Package::GetVersion(void) {
  return std::string(PACKAGE_VERSION);
}

const std::string Package::GetName(void) { return std::string("NetPanzer"); }

const std::string Package::GetFullyQualifiedName(void) {
  return Package::GetName() + " " + Package::GetVersion();
}

#else

#include "package.hpp"
#include "test.hpp"

int main(void)
{
  int len = Package::GetVersion().length();
  assert(len >= 5 && len < 10);
  assert(Package::GetName().compare("NetPanzer") == 0);

  return 0;
}

#endif
