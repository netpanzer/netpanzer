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

#ifndef __PACKAGE_HPP
#define __PACKAGE_HPP

#include <filesystem>
#include <string>

#include "config.h"

class Package {
 public:
  static const std::string getVersion(void);
  static const std::string getFormalName(void);
  static const std::string getBinName(void);
  static const std::string getFullyQualifiedName(void);

  static const std::filesystem::path getDataDir(void) { return dataDir; }
  static const std::string getLocaleDir(void) { return localeDir; }
  static void assignDataDir(void);
  static void assignLocaleDir(void);
  static void setDataDir(std::filesystem::path &absPath) { dataDir = absPath; }
  static void setLocaleDir(std::filesystem::path absPath) { localeDir = absPath; }

 private:
  inline static std::filesystem::path dataDir;
  inline static std::filesystem::path localeDir;
};

#endif
