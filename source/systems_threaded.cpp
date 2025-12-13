#include <mutex>
#include "optick.h"
#include "systems.h"

#include "systems_threaded.h"

std::mutex g_worldMutex;

#define DEFINE_THREADED_SYSTEM(fn)                                    \
void fn##_ts (World &world, float dt)                                 \
{                                                                     \
    OPTICK_THREAD(#fn "_thread");                                     \
    std::unique_lock<std::mutex> lock(g_worldMutex, std::defer_lock); \
    {                                                                 \
        OPTICK_EVENT("MutexWait");                                    \
        lock.lock();                                                  \
    }                                                                 \
    OPTICK_EVENT();                                                   \
    fn(world, dt);                                                    \
}

#define X(system) DEFINE_THREADED_SYSTEM(system)
    SYSTEMS_LIST
#undef X
