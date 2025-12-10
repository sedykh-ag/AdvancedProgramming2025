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
#include "fsm.h"
#include "states.h"
#include "transitions.h"
#include "shared_helpers.h"
#include "bt.h"
#include "init_world.h"


const int BotPopulationCount = 100;
const float PredatorProbability = 0.2f;
const int InitialFoodAmount = 100;
extern const int PredatorCloseTriggerDist = 4;

StateMachine get_peasant_sm() {
    StateMachine sm{};

    int find = sm.addState(create_find_closest_food_state());
    int plan = sm.addState(create_plan_path_state());
    int execute = sm.addState(create_execute_planned_path_state());
    int avoid = sm.addState(create_avoid_predators_state());

    sm.addTransition(create_has_destination_transition(), find, plan);
    sm.addTransition(create_has_path_transition(), plan, execute);
    sm.addTransition(
        create_negate_transition(create_has_path_transition()), execute, find
    );

    sm.addTransition(create_predator_close_transition(), find, avoid);
    sm.addTransition(create_predator_close_transition(), plan, avoid);
    sm.addTransition(create_predator_close_transition(), execute, avoid);

    sm.addTransition(create_negate_transition(create_predator_close_transition()), avoid, find);

    return sm;
}

StateMachine get_predator_sm() {
    StateMachine sm{};

    int find = sm.addState(create_find_closest_peasant_state());
    int plan = sm.addState(create_plan_path_state());
    int execute = sm.addState(create_execute_planned_path_state());

    sm.addTransition(create_has_destination_transition(), find, plan);
    sm.addTransition(create_has_path_transition(), plan, execute);
    sm.addTransition(
        create_negate_transition(create_has_path_transition()), execute, find
    );

    return sm;
}

std::shared_ptr<RootNode> get_peasant_bt() {
    // conditions
    auto predatorClose = std::make_shared<ConditionNode>(
        [](int entity_idx, World &world, float) {
            auto &myTransform = world.characters.transforms[entity_idx];
            int2 myPos = {(int)myTransform.x,(int) myTransform.y};
            int2 closestPredatorPos = locate_closest_predator(world.characters, myPos);

            return dist(myPos, closestPredatorPos) <= PredatorCloseTriggerDist;
        },
        "predatorClose"
    );
    auto hasPath = std::make_shared<ConditionNode>(
        [](int entity_idx, World &world, float) {
            return !world.characters.paths[entity_idx].empty();
        },
        "hasPath"
    );

    // actions
    auto avoidPredators = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            avoid_predators(entity_idx, world, dt);
            return Status::SUCCESS;
        },
        "avoidPredators"
    );
    auto resetPath = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            world.characters.paths[entity_idx] = std::stack<int2>();
            return Status::SUCCESS;
        },
        "resetPath"
    );

    auto stepAlongPath = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            const auto &path = world.characters.paths[entity_idx];
            if (path.empty())
                return Status::FAILURE;

            execute_planned_path(entity_idx, world, dt);
            return Status::SUCCESS;
        },
        "stepAlongPath"
    );

    auto findClosestFood = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            bool found = find_closest_food(entity_idx, world);
            return found ? Status::SUCCESS : Status::FAILURE;
        },
        "findClosestFood"
    );

    auto findClosestPeasantMate = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            if (world.characters.healths[entity_idx].current <= 90)
                return Status::FAILURE;

            bool found = find_closest_peasant(entity_idx, world);
            return found ? Status::SUCCESS : Status::FAILURE;
        },
        "findClosestPeasantMate"
    );

    auto planPath = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            bool planned = plan_path(entity_idx, world);
            return planned ? Status::SUCCESS : Status::FAILURE;
        },
        "planPath"
    );

    // sequences
    auto tryAvoidPredators = std::make_shared<SequenceNode>("tryAvoidPredators");
    auto tryMovingAlongPath = std::make_shared<SequenceNode>("tryMovingAlongPath");
    auto tryPlanNewPath = std::make_shared<SequenceNode>("tryPlanNewPath");

    tryAvoidPredators->addChild(predatorClose);
    tryAvoidPredators->addChild(avoidPredators);
    tryAvoidPredators->addChild(resetPath);

    tryMovingAlongPath->addChild(hasPath);
    tryMovingAlongPath->addChild(stepAlongPath);

    auto findDestination = std::make_shared<SelectorNode>("findDestination");
    findDestination->addChild(findClosestPeasantMate);
    findDestination->addChild(findClosestFood);

    tryPlanNewPath->addChild(findDestination);
    tryPlanNewPath->addChild(planPath);

    // root selector
    auto rootSelector = std::make_shared<SelectorNode>("rootSelector");
    rootSelector->addChild(tryAvoidPredators);
    rootSelector->addChild(tryMovingAlongPath);
    rootSelector->addChild(tryPlanNewPath);

    auto rootNode = std::make_shared<RootNode>(rootSelector);
    return rootNode;
}

std::shared_ptr<RootNode> get_predator_bt() {
    // conditions
    auto hasPath = std::make_shared<ConditionNode>(
        [](int entity_idx, World &world, float) {
            return !world.characters.paths[entity_idx].empty();
        },
        "hasPath"
    );

    // actions
    auto resetPath = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            world.characters.paths[entity_idx] = std::stack<int2>();
            return Status::SUCCESS;
        },
        "resetPath"
    );

    auto stepAlongPath = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            const auto &path = world.characters.paths[entity_idx];
            if (path.empty())
                return Status::FAILURE;

            execute_planned_path(entity_idx, world, dt);
            return Status::SUCCESS;
        },
        "stepAlongPath"
    );

    auto findClosestPeasant = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            bool found = find_closest_peasant(entity_idx, world);
            return found ? Status::SUCCESS : Status::FAILURE;
        },
        "findClosestPeasant"
    );

    auto findClosestPredatorMate = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            if (world.characters.healths[entity_idx].current <= 90)
                return Status::FAILURE;

            bool found = find_closest_predator(entity_idx, world);
            return found ? Status::SUCCESS : Status::FAILURE;
        },
        "findClosestPredatorMate"
    );

    auto planPath = std::make_shared<ActionNode>(
        [](int entity_idx, World &world, float dt) {
            bool planned = plan_path(entity_idx, world);
            return planned ? Status::SUCCESS : Status::FAILURE;
        },
        "planPath"
    );

    // sequences
    auto tryMovingAlongPath = std::make_shared<SequenceNode>("tryMovingAlongPath");
    auto tryPlanNewPath = std::make_shared<SequenceNode>("tryPlanNewPath");

    tryMovingAlongPath->addChild(hasPath);
    tryMovingAlongPath->addChild(stepAlongPath);

    auto findDestination = std::make_shared<SelectorNode>("findDestination");
    findDestination->addChild(findClosestPredatorMate);
    findDestination->addChild(findClosestPredatorMate);

    tryPlanNewPath->addChild(findDestination);
    tryPlanNewPath->addChild(planPath);

    // root selector
    auto rootSelector = std::make_shared<SelectorNode>("rootSelector");
    rootSelector->addChild(tryMovingAlongPath);
    rootSelector->addChild(tryPlanNewPath);

    auto rootNode = std::make_shared<RootNode>(rootSelector);
    return rootNode;
}

std::unique_ptr<TileSet> tilesetPtr{nullptr};

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
    tilesetPtr = std::make_unique<TileSet>(tilemap);

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
                world.cells.add(
                    tilesetPtr->get_tile(spriteName),
                    {(double)j, (double)i}
                );
            }
        }

    {
        auto heroPos = dungeon.getRandomFloorPosition();
        world.characters.add(
            tilesetPtr->get_tile("knight"),
            {(double)heroPos.x, (double)heroPos.y},
            100,
            100,
            0.0f,
            StateMachine{},
            {-1, -1},
            std::stack<int2>{},
            nullptr,
            true,
            false
        );
        world.camera.transform = {(double)heroPos.x, (double)heroPos.y};
    }


    for (int e = 0; e < BotPopulationCount; ++e) {
        auto enemyPos = dungeon.getRandomFloorPosition();
        const bool isPredator = (rand() % 100) < int(PredatorProbability * 100.f);

        world.characters.add(
            isPredator ? tilesetPtr->get_tile("ghost") : tilesetPtr->get_tile("peasant"),
            {(double)enemyPos.x, (double)enemyPos.y},
            100,
            100,
            0,
            isPredator ? get_predator_sm() : get_peasant_sm(),
            {-1, -1},
            std::stack<int2>{},
            isPredator ? get_predator_bt() : get_peasant_bt(),
            false,
            isPredator
        );
    }

    {
        world.foodFabriques.add(tilesetPtr->get_tile("health_small"), 10, 0, 100, 0);
        world.foodFabriques.add(tilesetPtr->get_tile("health_large"), 25, 0, 30, 0);
        world.foodFabriques.add(tilesetPtr->get_tile("stamina_small"), 0, 10, 35, 0);
        world.foodFabriques.add(tilesetPtr->get_tile("stamina_large"), 0, 25, 20, 0);
    }

    {
        world.foodGenerator = std::make_unique<FoodGenerator>(world.dungeon, world.foodFabriques, world.foods, 2.0f / RoomAttempts);
        for (int i = 0; i < InitialFoodAmount; i++)
            world.foodGenerator->generate_random_food();
    }
}