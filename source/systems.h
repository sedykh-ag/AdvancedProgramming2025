#pragma once
#include "stamina.h"
#include "dungeon_generator.h"
#include "math2d.h"


class World;

float character_speed(const Stamina &stamina);
bool character_can_pass(const Dungeon &dungeon, const int2 p);

#define SYSTEMS_LIST       \
    X(hero_input_system)   \
    X(npc_walk_system)     \
    X(npc_predator_system) \
    X(food_consume_system) \
    X(starvation_system)   \
    X(tiredness_system)    \
    X(npc_sm_system)       \
    X(npc_bt_system)       \
    X(reproduction_system) \

#define CORE_SYSTEMS_LIST  \
    X(hero_input_system)   \
    X(npc_predator_system) \
    X(food_consume_system) \
    X(starvation_system)   \
    X(tiredness_system)    \
    X(reproduction_system) \
    X(reproduction_system) \
    X(npc_bt_system)       \

#define X(system) void system(World &world, float dt);
    SYSTEMS_LIST
#undef X
