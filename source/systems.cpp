#include "dungeon_generator.h"
#include "stamina.h"
#include "world.h"
#include "math2d.h"
#include "systems.h"

static float character_speed(const Stamina &stamina) {
    return 5.0f + ((stamina.current <= 0.0f) ? 0.0f : 5.0f);
}

static bool character_can_pass(const Dungeon &dungeon, const int2 p) {
    const auto &grid = dungeon.getGrid();
    if (p.x < 0 || p.y < 0 || p.y >= (int)grid.size() || p.x >= (int)grid[0].size())
        return false;
    return grid[p.y][p.x] == Dungeon::FLOOR;
}

void hero_input_system(World &world, float dt) {
    for (int i = 0; i < world.characters.transforms.size(); i++)
    {
        if (!world.characters.isHero[i]) continue;

        const bool* keys = SDL_GetKeyboardState(nullptr);
        auto &transform = world.characters.transforms[i];
        auto &stamina = world.characters.staminas[i];
        float &timeSinceLastMove = world.characters.timeSinceLastMove[i];
        float cellPerSecond = character_speed(stamina);

        int2 intDelta;
        bool moved = false;
        if (keys[SDL_SCANCODE_W]) { intDelta.y -= 1; moved = true; }
        if (keys[SDL_SCANCODE_S]) { intDelta.y += 1; moved = true; }
        if (keys[SDL_SCANCODE_A]) { intDelta.x -= 1; moved = true; }
        if (keys[SDL_SCANCODE_D]) { intDelta.x += 1; moved = true; }
        if (!moved)
            continue;
        if (moved && (timeSinceLastMove < 1.f / cellPerSecond)) {
            timeSinceLastMove += dt;
            continue;
        }
        timeSinceLastMove = 0.f;
        int2 newPos = int2(transform.x + intDelta.x, transform.y + intDelta.y);
        if (character_can_pass(world.dungeon, newPos)) {
            transform.x = newPos.x;
            transform.y = newPos.y;
            world.camera.transform = transform;
        }
    }
}

void npc_walk_system(World &world, float dt) {
    const float moveCooldown = 1.0f;
    for (int i = 0; i < world.characters.transforms.size(); i++)
    {
        if (world.characters.isHero[i]) continue;

        auto &transform = world.characters.transforms[i];
        auto &stamina = world.characters.staminas[i];
        float &timeSinceLastMove = world.characters.timeSinceLastMove[i];

        timeSinceLastMove += dt * character_speed(stamina);
        if (timeSinceLastMove < 1.0f)
            continue;
        timeSinceLastMove -= 1.0f;

        // try to move in a random direction
        const int2 directions[] = {int2
            {1,0},
            int2{-1,0},
            int2{0,1},
            int2{0,-1},
        };
        int r = rand() % 4;
        int2 intDelta = directions[r];
        int2 newPos = int2((int)transform.x + intDelta.x, (int)transform.y + intDelta.y);

        if (character_can_pass(world.dungeon, newPos)) {
            transform.x = newPos.x;
            transform.y = newPos.y;
            timeSinceLastMove = 0.0f;
        }
    }
}

void npc_predator_system(World &world) {

}

void food_consume_system(World &world) {

}