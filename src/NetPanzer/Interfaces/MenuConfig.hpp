#ifndef __MENUCONFIG_HPP
#define __MENUCONFIG_HPP

#include <string>
#include "Util/NoCopy.hpp"
#include "Core/CoreTypes.hpp"


class MenuConfig : public NoCopy {
 public:

  static void loadConfig();

  static Uint8 bar_solid_color;
  static Uint8 bar_border_color;
  static NPString* font;
 
 private:
  friend class ScriptManager;
  static void registerScript(const NPString& table_name);
};

#endif
