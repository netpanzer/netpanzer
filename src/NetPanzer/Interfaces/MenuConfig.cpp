#include "2D/Color.hpp"
#include "Interfaces/MenuConfig.hpp"
#include "Scripts/ScriptHelper.hpp"
#include "Scripts/ScriptManager.hpp"

#include "Util/Log.hpp"


const static char* luaconfigfile = "scripts/mainmenu.cfg";

Uint8 MenuConfig::bar_solid_color = 0;
Uint8 MenuConfig::bar_border_color = 0;
NPString* MenuConfig::font = 0;

static const ScriptVarBindRecord bar_getters[] = {
    {"solid_color", GETSVTYPE_BYTE, &MenuConfig::bar_solid_color},
    {"border_color", GETSVTYPE_BYTE, &MenuConfig::bar_border_color},
    {"font", GETSVTYPE_STRING, &MenuConfig::font},
    {0, 0}
};

static const ScriptVarBindRecord bar_setters[] = {
    {"solid_color", SETSVTYPE_BYTE, &MenuConfig::bar_solid_color},
    {"border_color", SETSVTYPE_BYTE, &MenuConfig::bar_border_color},
    {"font", SETSVTYPE_STRING, &MenuConfig::font},
    {0, 0}
};

void MenuConfig::loadConfig() {
  ScriptManager::loadConfigFile(luaconfigfile, "menu", true);
}

void MenuConfig::registerScript(const NPString& table_name) {
  if (!font) {
    font = new NPString("");
  }
  
  ScriptManager::bindStaticVariables(table_name + ".bar",
                                     "MenuBarMetaTable", bar_getters,
                                     bar_setters);
  Color::bindColorsReadonly(table_name + ".color", "MenuBarColorMetaTable");
}
