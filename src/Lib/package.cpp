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
#include "Util/Log.hpp"

#include <filesystem>
#include <fstream>
#include <vector>

const std::string Package::getVersion(void) {
  return std::string(PACKAGE_VERSION);
}

const std::string Package::getFormalName(void) { return std::string("NetPanzer"); }
const std::string Package::getBinName(void) { return std::string("netpanzer"); }

const std::string Package::getFullyQualifiedName(void) {
  return getFormalName() + " " + getVersion();
}

void Package::assignLocaleDir(void) {
  char *npLocEnv = getenv("NETPANZER_LOCALEDIR");
  if (npLocEnv != NULL) {
    std::ifstream file(std::string(npLocEnv) + "/de");
    if (file.good()) setLocaleDir(npLocEnv);
    else LOGGER.debug("localeDir: %s\n", Package::getLocaleDir().c_str());
    //else LOGGER.warn("fprintf
  }
  else setLocaleDir(NP_LOCALEDIR);

  LOGGER.warning("localeDir: %s\n", Package::getLocaleDir().c_str());

  return;
}

void Package::assignDataDir(void) {
  std::vector<std::filesystem::path> possible{NP_DATADIR};

  char *npDataEnv = getenv("NETPANZER_DATADIR");
  if (npDataEnv != NULL) possible.insert(possible.begin(), npDataEnv);
  int length = possible.size();

  int i = 0;
  while (i < length) {
    std::filesystem::path tmp_path = possible[i];
    tmp_path /= "maps";
    std::ifstream file(tmp_path);
    if (file.good()) {
      setDataDir(possible[i]);
      return;
    }

    i++;
  }

  return;
}

#else

#include <string.h>

#include "test.hpp"

void test_name(void) {
  int len = Package::getVersion().length();
  assert(len >= 5 && len < 14);
  assert(Package::getFormalName().compare("NetPanzer") == 0);

  return;
}

void test_datadir(void) {
  Package::assignDataDir();
  const char *tmp = MESON_SOURCE_ROOT;

  // Workaround for PR #141
  const char *cirrusOS = getenv("CIRRUS_OS");
  if (cirrusOS != NULL) {
    if (strcmp(cirrusOS, "darwin") == 0) {
      fputs("CIRRUS_WORKING_DIR used\n", stderr);
      const char *envCirrusWd = getenv("CIRRUS_WORKING_DIR");
      if (envCirrusWd != NULL) tmp = envCirrusWd;
    }
  }

  fprintf(stderr, "datadir: %s\nmeson_source_root: %s\n", Package::getDataDir().c_str(),
    tmp);
  char expect_data[strlen(tmp) + sizeof "/data"];
  snprintf(expect_data, sizeof expect_data, "%s/data", tmp);
  assert(Package::getDataDir() == expect_data);

  return;
}

// main() must be defined with the args in this format, otherwise we may get an
// "undefined reference to SDL_main"
int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  test_name();
  test_datadir();
  return 0;
}

#endif
