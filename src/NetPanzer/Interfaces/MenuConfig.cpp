#include "Interfaces/MenuConfig.hpp"
#include "Scripts/ScriptHelper.hpp"
#include "Scripts/ScriptManager.hpp"

#include "Util/Log.hpp"

NPString* MenuConfig::bar_solid_color = 0;

const static char* luaconfigfile = "scripts/mainmenu.cfg";

static const ScriptVarBindRecord bar_getters[] = {
    {"solid_color", GETSVTYPE_STRING, &MenuConfig::bar_solid_color},
    {0, 0}
};

static const ScriptVarBindRecord bar_setters[] = {
    {"solid_color", SETSVTYPE_STRING, &MenuConfig::bar_solid_color},
    {0, 0}
};

void MenuConfig::loadConfig() {
  ScriptManager::loadConfigFile(luaconfigfile, "menu", true);
}

void MenuConfig::registerScript(const NPString& table_name) {

  if (!bar_solid_color) {
    bar_solid_color = new NPString("");
  }

  ScriptManager::bindStaticVariables(table_name + ".bar",
                                     "MenuBarMetaTable", bar_getters,
                                     bar_setters);
}
