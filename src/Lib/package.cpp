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

#include <cstdlib>
#include <fstream>
#include <vector>

#ifndef PACKAGE_VERSION
#include "config.h"
#endif

#ifdef NP_DATADIR
#define DATADIR NP_DATADIR
#endif

const std::string Package::GetVersion(void) {
  return std::string(PACKAGE_VERSION);
}

const std::string Package::GetName(void) { return std::string("NetPanzer"); }

const std::string Package::GetFullyQualifiedName(void) {
  return Package::GetName() + " " + Package::GetVersion();
}

void Package::assignDataDir(void) {
  std::vector<std::string> possible{".", "..", DATADIR};

  char *npDataEnv = getenv("NETPANZER_DATADIR");
  if (npDataEnv != NULL) possible.insert(possible.begin(), npDataEnv);
  char *npAppDir = getenv("APPDIR");
  if (npAppDir != NULL) possible.insert(possible.end(), npAppDir);
  int length = possible.size();

  int i = 0;
  while (i < length) {
    std::ifstream file(possible[i] + "/maps");
    if (file.good()) {
      std::filesystem::path p = possible[i];
      Package::setDataDir(std::filesystem::absolute(p).parent_path().parent_path());
      return;
    }

    i++;
  }

  // check env for $APPDIOR

  return;
}

const std::string Package::getDataSubPath(const char *data) {
  std::string subPath = Package::getDataDir() + "/" + std::string(data);
  return subPath;
}

#else

#include <filesystem>

#include "package.hpp"
#include "test.hpp"

void test_name(void) {
  int len = Package::GetVersion().length();
  assert(len >= 5 && len < 10);
  assert(Package::GetName().compare("NetPanzer") == 0);

  return;
}

void test_datadir(void) {
  // fprintf(stderr, "meson_build_root: " MESON_BUILD_ROOT "\n");
  std::filesystem::current_path(std::string(MESON_BUILD_ROOT));
  // fprintf(stderr, "cwd: %s\n", path.c_str());
  Package::assignDataDir();
  fprintf(stderr, "datadir: %s\n", Package::getDataDir().c_str());
  assert(Package::getDataDir() == MESON_SOURCE_ROOT);
  assert(Package::getDataSubPath("maps/") == MESON_SOURCE_ROOT "/maps/");

  return;
}

int main(void) {
  test_name();
  test_datadir();
  return 0;
}

#endif
