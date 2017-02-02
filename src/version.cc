#include "version.h"

#include <string>

#include "version_defs.h"

namespace sidewinder {
static const char* VERSION =
    "SIDEWINDER_VERSION " SIDEWINDER_VERSION " " SIDEWINDER_COMPILE_DATE
    " " SIDEWINDER_COMPILE_TIME;
} /* sidewinder */

using namespace sidewinder;
std::string GetVersion() { return std::string(VERSION); }
