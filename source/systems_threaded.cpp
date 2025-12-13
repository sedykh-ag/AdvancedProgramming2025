#include "optick.h"
#include "systems.h"

#include "spinlock_mutex.h"
#include "systems_threaded.h"

spinlock_mutex g_worldMutex;

#define DEFINE_THREADED_SYSTEM(fn)                     \
void fn##_ts (World &world, float dt)                  \
{                                                      \
    {                                                  \
        OPTICK_EVENT("MutexWait");                     \
        g_worldMutex.lock(spinlock_idle_opts::noop{}); \
    }                                                  \
    OPTICK_EVENT();                                    \
    fn(world, dt);                                     \
    g_worldMutex.unlock();                             \
}

#define X(system) DEFINE_THREADED_SYSTEM(system)
    SYSTEMS_LIST
#undef X
