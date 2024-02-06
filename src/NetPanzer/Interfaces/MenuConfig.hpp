#ifndef __MENUCONFIG_HPP
#define __MENUCONFIG_HPP

#include <string>
#include "Util/NoCopy.hpp"
#include "Core/CoreTypes.hpp"


class MenuConfig : public NoCopy {
 public:

  static void loadConfig();

  static NPString* bar_solid_color;
 
 private:
  friend class ScriptManager;
  static void registerScript(const NPString& table_name);
};

#endif
