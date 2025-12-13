#pragma ocne
#include "systems.h"

#define X(system) void system##_ts(World &world, float dt);
    SYSTEMS_LIST
#undef X
