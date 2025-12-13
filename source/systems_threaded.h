#pragma ocne
#include "systems.h"

#define X(system) void system_ts(World &world, float dt);
    SYSTEMS_LIST
#undef X
