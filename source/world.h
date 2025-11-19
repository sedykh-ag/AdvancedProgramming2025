#pragma once

#include "dungeon_generator.h"
#include "food_generator.h"
#include "game_object.h"
#include "archetypes.h"
#include "systems.h"
#include <memory>
#include <vector>


const int LevelWidth = 120;
const int LevelHeight = 50;
const int RoomAttempts = 100;


class World : public std::enable_shared_from_this<World> {
public:
    std::shared_ptr<GameObject> create_object() {
        auto obj = std::make_shared<GameObject>();
        obj->world = shared_from_this();
        delayedAdd.push_back(obj);
        return obj;
    }

    void destroy_object(std::shared_ptr<GameObject> obj) {
        delayedRemove.push_back(obj);
    }

    void update(float dt) {
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

        foodGenerator->on_update(dt);
        hero_input_system(*this, dt);
        npc_walk_system(*this, dt);
        npc_predator_system(*this);
        food_consume_system(*this);
    }

    const std::vector<std::shared_ptr<GameObject>>& get_objects() const {
        return objects;
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

private:
    std::vector<std::shared_ptr<GameObject>> objects, delayedRemove, delayedAdd;
};
