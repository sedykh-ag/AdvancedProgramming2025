#include <unordered_set>
#include <cassert>

#include "world.h"
#include "systems.h"
#include "astar.h"
#include "shared_helpers.h"


int2 locate_closest_food(const Foods &foods, const int2 start) {
    int2 closest = {-1, -1};
    int min_dist = std::numeric_limits<int>::max();
    for (const auto &transform : foods.transforms) {
        int2 food_pos{(int)transform.x, (int)transform.y};
        int d = dist(start, food_pos);

        if (d < min_dist) {
            min_dist = d;
            closest = food_pos;
        }
    }
    return closest;
}

int2 locate_closest_predator(const Characters &chars, const int2 start) {
    int2 closest = {-1, -1};
    int min_dist = std::numeric_limits<int>::max();
    for (int i = 0; i < chars.transforms.size(); i++) {
        if (!chars.isPredator[i])
            continue;

        const auto& predatorTransform = chars.transforms[i];
        int2 predatorPos{(int)predatorTransform.x, (int)predatorTransform.y};

        if (predatorPos == start)
            continue;

        int d = dist(start, predatorPos);
        if (d < min_dist) {
            min_dist = d;
            closest = predatorPos;
        }
    }
    return closest;
}

int2 locate_closest_peasant(const Characters &chars, const int2 start) {
    int2 closest = {-1, -1};
    int min_dist = std::numeric_limits<int>::max();
    for (int i = 0; i < chars.transforms.size(); i++) {
        if (chars.isPredator[i])
            continue;

        const auto& peasantTransform = chars.transforms[i];
        int2 peasantPos{(int)peasantTransform.x, (int)peasantTransform.y};

        if (peasantPos == start)
            continue;

        int d = dist(start, peasantPos);
        if (d < min_dist) {
            min_dist = d;
            closest = peasantPos;
        }
    }
    return closest;
}

bool find_closest_food(int entity_idx, World &world) {
    auto transform = world.characters.transforms[entity_idx];
    auto &destination = world.characters.destinations[entity_idx];
    destination = locate_closest_food(world.foods, {(int)transform.x, (int)transform.y});
    return destination != int2{-1, -1};
}

bool find_closest_peasant(int entity_idx, World &world) {
    auto transform = world.characters.transforms[entity_idx];
    auto &destination = world.characters.destinations[entity_idx];
    destination = locate_closest_peasant(world.characters, {(int)transform.x, (int)transform.y});
    return destination != int2{-1, -1};
}

bool find_closest_predator(int entity_idx, World &world) {
    auto transform = world.characters.transforms[entity_idx];
    auto &destination = world.characters.destinations[entity_idx];
    destination = locate_closest_predator(world.characters, {(int)transform.x, (int)transform.y});
    return destination != int2{-1, -1};
}

bool plan_path(int entity_idx, World &world) {
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
    return !path.empty();
}

static bool can_move(Characters &chars, int entity_idx, float dt) {
    const auto &stamina = chars.staminas[entity_idx];
    float &timeSinceLastMove = chars.timeSinceLastMove[entity_idx];
    timeSinceLastMove += dt * character_speed(stamina);
    if (timeSinceLastMove < 1.0f)
        return false;
    timeSinceLastMove -= 1.0f;
    return true;
}

void execute_planned_path(int entity_idx, World &world, float dt) {
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

void avoid_predators(int entity_idx, World &world, float dt) {
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
