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
    static float timeSinceLastMove = 0.0f;

    for (int i = 0; i < world.characters.transforms.size(); i++)
    {
        const bool* keys = SDL_GetKeyboardState(nullptr);

        if (!world.characters.isHero[i]) continue;
        auto &transform = world.characters.transforms[i];
        auto &stamina = world.characters.staminas[i];

        float cellPerSecond = character_speed(stamina);
        float moveCooldown = 1.0f / cellPerSecond;

        int2 intDelta;
        if (keys[SDL_SCANCODE_W]) intDelta.y -= 1;
        if (keys[SDL_SCANCODE_S]) intDelta.y += 1;
        if (keys[SDL_SCANCODE_A]) intDelta.x -= 1;
        if (keys[SDL_SCANCODE_D]) intDelta.x += 1;

        int2 newPos = int2(transform.x + intDelta.x, transform.y + intDelta.y);
        if (character_can_pass(world.dungeon, newPos) && (timeSinceLastMove > moveCooldown)) {
            transform.x = newPos.x;
            transform.y = newPos.y;
            world.camera.transform = transform;
            timeSinceLastMove = 0.0f;
        }
    }

    timeSinceLastMove += dt;
}

void food_consume_system(World &world) {

}