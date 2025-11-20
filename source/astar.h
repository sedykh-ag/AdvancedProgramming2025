#pragma once

#include <unordered_set>
#include <vector>

#include "dungeon_generator.h"
#include "math2d.h"

using Grid = std::vector<std::vector<Dungeon::Tile>>;

std::vector<int2> astar(Grid grid, const std::unordered_set<int2> &obstacles, int2 start, int2 goal);
