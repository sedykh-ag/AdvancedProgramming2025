#pragma once

#include <memory>
#include <vector>

#include "dungeon_generator.h"
#include "food_generator.h"
#include "archetypes.h"
#include "systems.h"
#include "systems_threaded.h"
#include "camera2d.h"


const int LevelWidth = 120;
const int LevelHeight = 50;
const int RoomAttempts = 100;


class World : public std::enable_shared_from_this<World> {
private:
    void do_delayed_removes() {
        std::sort(charactersDelayedRemove.begin(), charactersDelayedRemove.end(), std::greater<size_t>());
        charactersDelayedRemove.erase(
            std::unique(charactersDelayedRemove.begin(), charactersDelayedRemove.end()), charactersDelayedRemove.end()
        );
        for (const size_t index : charactersDelayedRemove)
            characters.remove(index);
        charactersDelayedRemove.clear();

        std::sort(foodsDelayedRemove.begin(), foodsDelayedRemove.end(), std::greater<size_t>());
        foodsDelayedRemove.erase(
            std::unique(foodsDelayedRemove.begin(), foodsDelayedRemove.end()), foodsDelayedRemove.end()
        );
        for (const size_t index : foodsDelayedRemove)
            foods.remove(index);
        foodsDelayedRemove.clear();
    }
public:
    void update(float dt) {
        do_delayed_removes();

        foodGenerator->on_update(dt);
        hero_input_system(*this, dt);
        npc_predator_system(*this, dt);
        food_consume_system(*this, dt);
        starvation_system(*this, dt);
        tiredness_system(*this, dt);
        reproduction_system(*this, dt);

        // switch here between sm (statemachine) and bt (beh tree)
        // npc_sm_system(*this, dt);
        npc_bt_system(*this, dt);
    }

    Dungeon dungeon{LevelWidth, LevelHeight, RoomAttempts};
    Camera camera;
    Cells cells;

    Characters characters;
    std::vector<size_t> charactersDelayedRemove;

    Foods foods;
    std::vector<size_t> foodsDelayedRemove;

    FoodFabriques foodFabriques;
    std::unique_ptr<FoodGenerator> foodGenerator;
};
