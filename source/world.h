#pragma once

#include <memory>
#include <thread>
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

        #define X(system) system(*this, dt);
            CORE_SYSTEMS_LIST
        #undef X
    }

    void update_threaded(float dt) {
        do_delayed_removes();

        std::thread food_thread{[&](){ foodGenerator->on_update_ts(dt); }};

        #define X(system) std::thread{[&](){ system##_ts(*this, dt); }},
        std::thread threads[] = {
            CORE_SYSTEMS_LIST
        };
        #undef X

        food_thread.join();
        for (std::thread &t : threads)
            t.join();
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
