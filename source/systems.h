#pragma once

class World;

void hero_input_system(World &world, float dt);
void npc_walk_system(World &world, float dt);
void npc_predator_system(World &world);
void food_consume_system(World &world);
void starvation_system(World &world, float dt);
void tiredness_system(World &world, float dt);
