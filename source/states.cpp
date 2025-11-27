#include <cassert>
#include <unordered_set>

#include "math2d.h"
#include "transform2d.h"
#include "world.h"
#include "fsm.h"
#include "astar.h"
#include "systems.h"


class FindClosestFoodState : public State {
public:
    void on_enter(int entity_idx, World &world) override { }
    void on_exit(int entity_idx, World &world) override { }
    void act(int entity_idx, World &world, float dt) override {
        auto transform = world.characters.transforms[entity_idx];
        auto &destination = world.characters.destinations[entity_idx];
        destination = locate_closest_food(world.foods, {(int)transform.x, (int)transform.y});
    }
};

class FindClosestPeasantState : public State {
public:
    void on_enter(int entity_idx, World &world) override { }
    void on_exit(int entity_idx, World &world) override { }
    void act(int entity_idx, World &world, float dt) override {
        auto transform = world.characters.transforms[entity_idx];
        auto &destination = world.characters.destinations[entity_idx];
        destination = locate_closest_peasant(world.characters, {(int)transform.x, (int)transform.y});
    }
};

class PlanPathState : public State {
public:
    void on_enter(int entity_idx, World &world) override { }
    void on_exit(int entity_idx, World &world) override { }
    void act(int entity_idx, World &world, float dt) override {
        const auto &selfTransform = world.characters.transforms[entity_idx];
        const auto &destination = world.characters.destinations[entity_idx];
        auto &path = world.characters.paths[entity_idx];

        std::unordered_set<int2> obstacles;
        if (!world.characters.isPredator[entity_idx]) {
            for (int i = 0; i < world.characters.transforms.size(); i++) {
                if (world.characters.isPredator[i]) {
                    const auto &predatorTransform = world.characters.transforms[i];
                    obstacles.insert(int2{(int)predatorTransform.x, (int)predatorTransform.y});
                }
            }
        }

        path = astar(
            world.dungeon.getGrid(),
            obstacles,
            int2{(int)selfTransform.x, (int)selfTransform.y},
            destination
        );
    }
};

bool can_move(Characters &chars, int entity_idx, float dt) {
    const auto &stamina = chars.staminas[entity_idx];
    float &timeSinceLastMove = chars.timeSinceLastMove[entity_idx];
    timeSinceLastMove += dt * character_speed(stamina);
    if (timeSinceLastMove < 1.0f)
        return false;
    timeSinceLastMove -= 1.0f;
    return true;
}

class ExecutePlannedPathState : public State {
    const float moveCooldown = 1.0f;
public:
    void on_enter(int entity_idx, World &world) override { }
    void on_exit(int entity_idx, World &world) override {
        world.characters.destinations[entity_idx] = int2{-1,  -1};
        world.characters.paths[entity_idx] = std::stack<int2>();
    }
    void act(int entity_idx, World &world, float dt) override {
        auto &path = world.characters.paths[entity_idx];
        if (path.empty()) // reached the end of the path
            return;

        if (!can_move(world.characters, entity_idx, dt))
            return;

        auto &curTransform = world.characters.transforms[entity_idx];
        int2 curPos{(int)curTransform.x, (int)curTransform.y};

        int2 newPos = path.top();
        path.pop();

        assert(dist(curPos, newPos) <= 1);
        assert(character_can_pass(world.dungeon, newPos) && "astar should never be wrong");
        curTransform = {(double)newPos.x, (double)newPos.y};
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
        if (!can_move(world.characters, entity_idx, dt))
            return;

        auto &myTransform = world.characters.transforms[entity_idx];
        int2 myPos = {(int)myTransform.x,(int) myTransform.y};

        int2 closestPredatorPos = locate_closest_predator(world.characters, myPos);
        int2 awayDirection = myPos - closestPredatorPos;

        awayDirection = awayDirection.normalize();
        // forbid diagonal movement
        if (std::abs(awayDirection.x) + std::abs(awayDirection.y) > 1)
            awayDirection.y = 0;

        int2 newPos = myPos + awayDirection;
        if (!character_can_pass(world.dungeon, newPos))
            return;
        assert(dist(myPos, newPos) <= 1);
        auto newTransform = Transform2D{(double)newPos.x, (double)newPos.y};

        myTransform = newTransform;
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
