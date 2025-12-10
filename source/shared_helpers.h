#pragma once
#include "world.h"


int2 locate_closest_predator(const Characters &chars, const int2 start);
bool find_closest_food(int entity_idx, World &world);
bool find_closest_peasant(int entity_idx, World &world);
bool find_closest_predator(int entity_idx, World &world);
bool plan_path(int entity_idx, World &world);
void execute_planned_path(int entity_idx, World &world, float dt);
void avoid_predators(int entity_idx, World &world, float dt);
