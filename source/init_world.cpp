#include <iostream>
#include <memory>

#include "archetypes.h"
#include "world.h"
#include "image.h"
#include "dungeon_generator.h"
#include "transform2d.h"
#include "sprite.h"
#include "world.h"
#include "tileset.h"
#include "food_generator.h"


const int BotPopulationCount = 100;
const float PredatorProbability = 0.2f;
const int InitialFoodAmount = 100;


void init_world( SDL_Renderer* renderer, World& world)
{
    const int tileSize = 16;
    TexturePtr tilemap = LoadTextureFromFile("assets/kenney_tiny-dungeon/Tilemap/tilemap.png", renderer);
    std::vector<Sprite> sprites;
    if (!tilemap)
    {
        std::cerr << "Failed to load tilemap texture\n";
        return;
    }
    TileSet tileset(tilemap);
    // Возьмем несколько тайлов из тайлсета
    const std::vector<std::pair<int, int>> tileIndices = {
        {4, 0}, // dirty floor
        {4, 1}, // dirty floor
        {3, 4}, // wall
        {8, 1}, // knight
        {9, 0}, // ghost
    };
    for (const auto& [i, j] : tileIndices) {
        sprites.push_back(Sprite(
            tilemap,
            SDL_FRect{float(j * (tileSize + 1)), float(i * (tileSize + 1)), float(tileSize), float(tileSize)}
        ));
    }

    auto &dungeon = world.dungeon;
    const auto &grid = dungeon.getGrid();
    for (int i = 0; i < LevelHeight; ++i)
        for (int j = 0; j < LevelWidth; ++j)
        {
            const char * spriteName = nullptr;
            if (grid[i][j] == Dungeon::FLOOR) {
                spriteName = rand() % 2 == 0 ? "floor1" : "floor2";
            } else if (grid[i][j] == Dungeon::WALL) {
                spriteName = "wall";
            }
            if (spriteName) {
                world.cells.sprites.push_back(tileset.get_tile(spriteName));
                world.cells.transforms.push_back(Transform2D{(double)j, (double)i});
            }
        }

    {
        auto heroPos = dungeon.getRandomFloorPosition();
        world.characters.sprites.push_back(tileset.get_tile("knight"));
        world.characters.transforms.push_back({(double)heroPos.x, (double)heroPos.y});
        world.characters.healths.push_back(100);
        world.characters.staminas.push_back(100);
        world.characters.timeSinceLastMove.push_back(0.0f);
        world.characters.isHero.push_back(true);
        world.characters.isPredator.push_back(false);

        world.camera.transform = {(double)heroPos.x, (double)heroPos.y};
    }


    for (int e = 0; e < BotPopulationCount; ++e) {
        auto enemyPos = dungeon.getRandomFloorPosition();
        const bool isPredator = (rand() % 100) < int(PredatorProbability * 100.f);

        world.characters.sprites.push_back(isPredator ? tileset.get_tile("ghost") : tileset.get_tile("peasant"));
        world.characters.transforms.push_back({(double)enemyPos.x, (double)enemyPos.y});
        world.characters.healths.push_back(100);
        world.characters.staminas.push_back(100);
        world.characters.timeSinceLastMove.push_back(0.0f);
        world.characters.isHero.push_back(false);
        world.characters.isPredator.push_back(isPredator);
    }

    {
        world.foodFabriques.add(tileset.get_tile("health_small"), 10, 0, 100, 0);
        world.foodFabriques.add(tileset.get_tile("health_large"), 25, 0, 30, 0);
        world.foodFabriques.add(tileset.get_tile("stamina_small"), 0, 10, 35, 0);
        world.foodFabriques.add(tileset.get_tile("stamina_large"), 0, 25, 20, 0);
    }

    {
        world.foodGenerator = std::make_unique<FoodGenerator>(world.dungeon, world.foodFabriques, world.foods, 2.0f / RoomAttempts);
        for (int i = 0; i < InitialFoodAmount; i++)
            world.foodGenerator->generate_random_food();
    }
}