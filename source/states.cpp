#include <cassert>

#include "math2d.h"
#include "world.h"
#include "fsm.h"
#include "shared_helpers.h"


class FindClosestFoodState : public State {
public:
    void on_enter(int entity_idx, World &world) override { }
    void on_exit(int entity_idx, World &world) override { }
    void act(int entity_idx, World &world, float dt) override {
        find_closest_food(entity_idx, world);
    }
};

class FindClosestPeasantState : public State {
public:
    void on_enter(int entity_idx, World &world) override { }
    void on_exit(int entity_idx, World &world) override { }
    void act(int entity_idx, World &world, float dt) override {
        find_closest_peasant(entity_idx, world);
    }
};

class PlanPathState : public State {
public:
    void on_enter(int entity_idx, World &world) override { }
    void on_exit(int entity_idx, World &world) override { }
    void act(int entity_idx, World &world, float dt) override {
        plan_path(entity_idx, world);
    }
};

class ExecutePlannedPathState : public State {
    const float moveCooldown = 1.0f;
public:
    void on_enter(int entity_idx, World &world) override { }
    void on_exit(int entity_idx, World &world) override {
        world.characters.destinations[entity_idx] = int2{-1,  -1};
        world.characters.paths[entity_idx] = std::stack<int2>();
    }
    void act(int entity_idx, World &world, float dt) override {
        execute_planned_path(entity_idx, world, dt);
    }
};

class AvoidPredatorsState : public State {
public:
    void on_enter(int entity_idx, World &world) override {
        world.characters.destinations[entity_idx] = int2{-1,  -1};
        world.characters.paths[entity_idx] = std::stack<int2>{};
    }
    void on_exit(int entity_idx, World &world) override {
        world.characters.destinations[entity_idx] = int2{-1,  -1};
        world.characters.paths[entity_idx] = std::stack<int2>{};
    }
    void act(int entity_idx, World &world, float dt) override {
        avoid_predators(entity_idx, world, dt);
    }
};

class NoopState : public State {
    void on_enter(int entity_idx, World &world) override { }
    void on_exit(int entity_idx, World &world) override { }
    void act(int entity_idx, World &world, float dt) override { }
};


// state creators
State *create_find_closest_food_state() {
    return new FindClosestFoodState{};
}

State *create_find_closest_peasant_state() {
    return new FindClosestPeasantState{};
}

State *create_plan_path_state() {
    return new PlanPathState{};
}

State *create_execute_planned_path_state() {
    return new ExecutePlannedPathState{};
}

State *create_avoid_predators_state() {
    return new AvoidPredatorsState{};
}

State *create_noop_state() {
    return new NoopState{};
}
