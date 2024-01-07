
#include "globals.hpp"

#ifndef PACKAGE_VERSION
#include "config.h"
#endif

std::string GetVersion(void)
{
    return std::string(PACKAGE_VERSION);
}

std::string GetName(void)
{
    return std::string("NetPanzer");
}

std::string GetNameVersion(void)
{
    return GetName() + " " + GetVersion();
}
