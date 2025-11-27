#pragma once

#include <cstdint>
#include <vector>
#include <stack>
#include <utility>

#include "health.h"
#include "sprite.h"
#include "stamina.h"
#include "transform2d.h"
#include "fsm.h"
#include "math2d.h"


struct Cells {
    std::vector<Sprite> sprites;
    std::vector<Transform2D> transforms;

    void add(const Sprite &sprite, const Transform2D &transform) {
        sprites.push_back(sprite);
        transforms.push_back(transform);
    }

    void remove(size_t index) {
        sprites.erase(sprites.begin() + index);
        transforms.erase(transforms.begin() + index);
    }
};


struct Characters {
    std::vector<Sprite> sprites;
    std::vector<Transform2D> transforms;
    std::vector<Health> healths;
    std::vector<Stamina> staminas;
    std::vector<float> timeSinceLastMove;

    std::vector<StateMachine> stateMachines;
    std::vector<int2> destinations;
    std::vector<std::stack<int2>> paths;

    std::vector<uint8_t> isHero;
    std::vector<uint8_t> isPredator;

    void add(
        const Sprite &sprite,
        const Transform2D &transform,
        const Health &health,
        const Stamina &stamina,
        float timeSinceLastMove,
        StateMachine &&stateMachine,
        const int2 &destination,
        const std::stack<int2> &path,
        uint8_t isHero,
        uint8_t isPredator
    ) {
        sprites.push_back(sprite);
        transforms.push_back(transform);
        healths.push_back(health);
        staminas.push_back(stamina);
        this->timeSinceLastMove.push_back(timeSinceLastMove);
        stateMachines.push_back(std::move(stateMachine));
        destinations.push_back(destination);
        paths.push_back(path);
        this->isHero.push_back(isHero);
        this->isPredator.push_back(isPredator);
    }

    void remove(size_t index) {
        sprites.erase(sprites.begin() + index);
        transforms.erase(transforms.begin() + index);
        healths.erase(healths.begin() + index);
        staminas.erase(staminas.begin() + index);
        this->timeSinceLastMove.erase(this->timeSinceLastMove.begin() + index);
        stateMachines.erase(stateMachines.begin() + index);
        destinations.erase(destinations.begin() + index);
        paths.erase(paths.begin() + index);
        this->isHero.erase(this->isHero.begin() + index);
        this->isPredator.erase(this->isPredator.begin() + index);
    }
};

struct FoodFabriques {
    std::vector<Sprite> sprites;
    std::vector<int> healthRestore;
    std::vector<int> staminaRestore;
    std::vector<int> weightValue;
    std::vector<float> timeSinceLastSpawn;

    void add(
        const Sprite &sprite,
        int healthRestore,
        int staminaRestore,
        int weightValue,
        float timeSinceSpawn
    ) {
        sprites.push_back(sprite);
        this->healthRestore.push_back(healthRestore);
        this->staminaRestore.push_back(staminaRestore);
        this->weightValue.push_back(weightValue);
        this->timeSinceLastSpawn.push_back(timeSinceSpawn);
    }

    void remove(size_t index) {
        sprites.erase(sprites.begin() + index);
        this->healthRestore.erase(this->healthRestore.begin() + index);
        this->staminaRestore.erase(this->staminaRestore.begin() + index);
        this->weightValue.erase(this->weightValue.begin() + index);
        this->timeSinceLastSpawn.erase(this->timeSinceLastSpawn.begin() + index);
    }
};

struct Foods {
    std::vector<Sprite> sprites;
    std::vector<Transform2D> transforms;
    std::vector<int> healthRestore;
    std::vector<int> staminaRestore;

    void add(
        const Sprite &sprite,
        const Transform2D &transform,
        int healthRestore,
        int staminaRestore
    ) {
        sprites.push_back(sprite);
        transforms.push_back(transform);
        this->healthRestore.push_back(healthRestore);
        this->staminaRestore.push_back(staminaRestore);
    }

    void remove(size_t index) {
        sprites.erase(sprites.begin() + index);
        transforms.erase(transforms.begin() + index);
        this->healthRestore.erase(this->healthRestore.begin() + index);
        this->staminaRestore.erase(this->staminaRestore.begin() + index);
    }
};
