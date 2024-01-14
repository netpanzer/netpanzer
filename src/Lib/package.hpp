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

#include <filesystem>
#include <string>

// This condition should get removed after the scons build
#ifndef NP_DATADIR
#include "config.h"
#endif

class Package {
 public:
  static const std::string GetVersion(void);
  static const std::string GetName(void);
  static const std::string GetFullyQualifiedName(void);

  // This will probably get removed and the datadir assignment
  // can happen in getDataSubPath
  static const std::string getDataDir(void) { return datadir; }

  static void assignDataDir(void);
  static void setDataDir(const std::filesystem::path& p) { datadir = p; }

  static const std::string getDataSubPath(const char *path);

 private:
  inline static std::string datadir;
};
