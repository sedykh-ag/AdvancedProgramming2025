#pragma once
#include "stamina.h"
#include "dungeon_generator.h"
#include "math2d.h"


class World;

float character_speed(const Stamina &stamina);
bool character_can_pass(const Dungeon &dungeon, const int2 p);

void hero_input_system(World &world, float dt);
void npc_walk_system(World &world, float dt);
void npc_predator_system(World &world);
void food_consume_system(World &world);
void starvation_system(World &world, float dt);
void tiredness_system(World &world, float dt);
void npc_sm_system(World &world, float dt);
void npc_bt_system(World &world, float dt);
