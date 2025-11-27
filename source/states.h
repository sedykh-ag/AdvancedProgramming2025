#pragma once
#include "fsm.h"


State *create_find_closest_food_state();
State *create_find_closest_peasant_state();
State *create_plan_path_state();
State *create_execute_planned_path_state();
State *create_avoid_predators_state();
State *create_noop_state();
