#include "2D/Color.hpp"
#include "Interfaces/MenuConfig.hpp"
#include "Scripts/ScriptHelper.hpp"
#include "Scripts/ScriptManager.hpp"

#include "Util/Log.hpp"


const static char* luaconfigfile = "scripts/mainmenu.cfg";

Uint8 MenuConfig::bar_solid_color = 0;
int MenuConfig::bar_radius = 0;
Uint8 MenuConfig::bar_border_color = 0;

Uint8 MenuConfig::client_solid_color = 0;
int MenuConfig::client_radius = 0;
Uint8 MenuConfig::client_border_color = 0;

NPString* MenuConfig::menu_background = NULL;
NPString* MenuConfig::menu_start_background = NULL;
Uint8 MenuConfig::menu_start_background_color = 0;
NPString* MenuConfig::menu_font = NULL;
Uint8 MenuConfig::menu_dbg_text_color = 0;
int MenuConfig::menu_font_size = 0;

Uint8 MenuConfig::button_normal_border = 0;
Uint8 MenuConfig::button_normal_highlight = 0;
Uint8 MenuConfig::button_hover_border = 0;
Uint8 MenuConfig::button_hover_highlight = 0;
Uint8 MenuConfig::button_pressed_border = 0;
Uint8 MenuConfig::button_pressed_highlight = 0;
NPString* MenuConfig::button_texture = NULL;


static const ScriptVarBindRecord bar_getters[] = {
    {"solid_color", GETSVTYPE_BYTE, &MenuConfig::bar_solid_color},
    {"radius", GETSVTYPE_BYTE, &MenuConfig::bar_radius},
    {"border_color", GETSVTYPE_BYTE, &MenuConfig::bar_border_color},
    {0, 0}
};

static const ScriptVarBindRecord bar_setters[] = {
    {"solid_color", SETSVTYPE_BYTE, &MenuConfig::bar_solid_color},
    {"radius", SETSVTYPE_BYTE, &MenuConfig::bar_radius},
    {"border_color", SETSVTYPE_BYTE, &MenuConfig::bar_border_color},
    {0, 0}
};

static const ScriptVarBindRecord client_getters[] = {
    {"solid_color", GETSVTYPE_BYTE, &MenuConfig::client_solid_color},
    {"radius", GETSVTYPE_BYTE, &MenuConfig::client_radius},
    {"border_color", GETSVTYPE_BYTE, &MenuConfig::client_border_color},
    {0, 0}
};

static const ScriptVarBindRecord client_setters[] = {
    {"solid_color", SETSVTYPE_BYTE, &MenuConfig::client_solid_color},
    {"radius", SETSVTYPE_BYTE, &MenuConfig::client_radius},
    {"border_color", SETSVTYPE_BYTE, &MenuConfig::client_border_color},
    {0, 0}
};

static const ScriptVarBindRecord menu_getters[] = {
    {"background", GETSVTYPE_STRING, &MenuConfig::menu_background},
    {"start_background", GETSVTYPE_STRING, &MenuConfig::menu_start_background},
    {"start_background_color", GETSVTYPE_BYTE, &MenuConfig::menu_start_background_color},
    {"font", GETSVTYPE_STRING, &MenuConfig::menu_font},
    {"dbg_text_color", GETSVTYPE_BYTE, &MenuConfig::menu_dbg_text_color},
    {"font_size", GETSVTYPE_BYTE, &MenuConfig::menu_font_size},
    {0, 0}
};

static const ScriptVarBindRecord menu_setters[] = {
    {"background", SETSVTYPE_STRING, &MenuConfig::menu_background},
    {"start_background", SETSVTYPE_STRING, &MenuConfig::menu_start_background},
    {"start_background_color", SETSVTYPE_BYTE, &MenuConfig::menu_start_background_color},
    {"font", SETSVTYPE_STRING, &MenuConfig::menu_font},
    {"dbg_text_color", SETSVTYPE_BYTE, &MenuConfig::menu_dbg_text_color},
    {"font_size", SETSVTYPE_BYTE, &MenuConfig::menu_font_size},
    {0, 0}
};

static const ScriptVarBindRecord button_getters[] = {
    {"normal_border", GETSVTYPE_BYTE, &MenuConfig::button_normal_border},
    {"normal_highlight", GETSVTYPE_BYTE, &MenuConfig::button_normal_highlight},
    {"hover_border", GETSVTYPE_BYTE, &MenuConfig::button_hover_border},
    {"hover_highlight", GETSVTYPE_BYTE, &MenuConfig::button_hover_highlight},
    {"pressed_border", GETSVTYPE_BYTE, &MenuConfig::button_pressed_border},
    {"pressed_highlight", GETSVTYPE_BYTE, &MenuConfig::button_pressed_highlight},
    {"texture", GETSVTYPE_STRING, &MenuConfig::button_texture},
    {0, 0}
};

static const ScriptVarBindRecord button_setters[] = {
    {"normal_border", SETSVTYPE_BYTE, &MenuConfig::button_normal_border},
    {"normal_highlight", SETSVTYPE_BYTE, &MenuConfig::button_normal_highlight},
    {"hover_border", SETSVTYPE_BYTE, &MenuConfig::button_hover_border},
    {"hover_highlight", SETSVTYPE_BYTE, &MenuConfig::button_hover_highlight},
    {"pressed_border", SETSVTYPE_BYTE, &MenuConfig::button_pressed_border},
    {"pressed_highlight", SETSVTYPE_BYTE, &MenuConfig::button_pressed_highlight},
    {"texture", SETSVTYPE_STRING, &MenuConfig::button_texture},
    {0, 0}
};

void MenuConfig::loadConfig() {
  ScriptManager::loadConfigFile(luaconfigfile, "menu");
}

void MenuConfig::registerScript(const NPString& table_name) {
  if (!menu_font) {
    // These aren't deallocated.
    // May cause a memory leak if registerScript()
    // is called more than once.
    menu_font = new NPString("");
  }
  
  if (!menu_background) {
    menu_background = new NPString("");
  }

  if (!menu_start_background) {
    menu_start_background = new NPString("");
  }

  if (!button_texture) {
    button_texture = new NPString("");
  }
  
  ScriptManager::bindStaticVariables(table_name + ".bar",
                                     "MenuBarMetaTable", bar_getters,
                                     bar_setters);
  
  ScriptManager::bindStaticVariables(table_name + ".client",
                                    "MenuClientMetaTable", client_getters,
                                    client_setters);
  
  ScriptManager::bindStaticVariables(table_name + ".menu",
                                    "MenuMenuMetaTable", menu_getters,
                                    menu_setters);
  
  ScriptManager::bindStaticVariables(table_name + ".button",
                                    "MenuButtonMetaTable", button_getters,
                                    button_setters);
  
  Color::bindColorsReadonly(table_name + ".color", "MenuBarColorMetaTable");
}
