#ifndef __MENUCONFIG_HPP
#define __MENUCONFIG_HPP

#include <string>
#include "Util/NoCopy.hpp"
#include "Core/CoreTypes.hpp"


class MenuConfig : public NoCopy {
 public:

  static void loadConfig();

  static Uint8 bar_solid_color;
  static int bar_radius;
  static Uint8 bar_border_color;

  static Uint8 client_solid_color;
  static int client_radius;
  static Uint8 client_border_color;

  static NPString* menu_background;
  static NPString* menu_start_background;
  static Uint8 menu_start_background_color;
  static NPString* menu_font;
  static Uint8 menu_dbg_text_color;
  static int menu_font_size;

  static Uint8 button_normal_border;
  static Uint8 button_normal_highlight;
  static Uint8 button_hover_border;
  static Uint8 button_hover_highlight;
  static Uint8 button_pressed_border;
  static Uint8 button_pressed_highlight;
  static NPString* button_texture;
 
 private:
  friend class ScriptManager;
  static void registerScript(const NPString& table_name);
};

#endif
