#pragma once

#include "optick.h"
#include "archetypes.h"
#include "dungeon_generator.h"
#include <mutex>

extern std::mutex g_worldMutex;

class FoodGenerator {
private:
    Dungeon &dungeon;
    const FoodFabriques &fabriques;
    Foods &foods;
    float timeSinceLastSpawn = 0.f; // seconds between spawns
    float spawnInterval = 1.f;
    int fabriquesProbabilitySum = 0;
public:

    FoodGenerator(Dungeon &dungeon, const FoodFabriques &fabriques, Foods &foods, float spawnInterval)
        : dungeon(dungeon), fabriques(fabriques), foods(foods), spawnInterval(spawnInterval)
    {
        for (const int weight : fabriques.weightValue)
            fabriquesProbabilitySum += weight;
    }

    void generate_random_food()
    {
        int2 position = dungeon.getRandomFloorPosition();
        int rand_value = rand() % fabriquesProbabilitySum;

        for (int i = 0; i < fabriques.weightValue.size(); i++) {
            int weight = fabriques.weightValue[i];
            if (rand_value < weight) {
                auto &sprite = fabriques.sprites[i];
                int healthRestore = fabriques.healthRestore[i];
                int staminaRestore = fabriques.staminaRestore[i];

                foods.add(sprite, {(double)position.x, (double)position.y}, healthRestore, staminaRestore);
                break;
            }
            rand_value -= weight;
        }
    }

    void on_update(float dt) {
        timeSinceLastSpawn += dt;
        if (timeSinceLastSpawn >= spawnInterval) {
            timeSinceLastSpawn = 0.f;
            // spawn food
            generate_random_food();
        }
    }

    void on_update_ts(float dt) {
        std::unique_lock<std::mutex> lock(g_worldMutex, std::defer_lock);
        {
            OPTICK_EVENT("MutexWait");
            lock.lock();
        }
        OPTICK_EVENT();
        on_update(dt);
    }
};
