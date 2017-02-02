#include "version.h"

// used for: strings steamworks | grep SIDEWINDER_VERSION
static const char* VERSION __attribute__((unused)) =
    "SIDEWINDER_VERSION " SIDEWINDER_VERSION " " SIDEWINDER_COMPILE_DATE
    " " SIDEWINDER_COMPILE_TIME;
