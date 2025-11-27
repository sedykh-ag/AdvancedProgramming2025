#include <unordered_set>
#include <stack>
#include <queue>
#include <limits>

#include "math2d.h"
#include "astar.h"


struct AStarNode {
    int2 pos;
    int g; // cost from start
    int f; // g + heuristic
};

struct AStarNodeCompare {
    bool operator()(const AStarNode& a, const AStarNode& b) const {
        // priority_queue puts the "largest" element on top,
        // so we reverse the comparison to get the smallest f.
        return a.f > b.f;
    }
};

std::stack<int2> astar(const Grid &grid, const std::unordered_set<int2> &obstacles, int2 start, int2 goal) {
    std::stack<int2> path;

    const int height = static_cast<int>(grid.size());
    if ((grid.size() == 0) || (grid[0].size() == 0)) {
        return path;
    }
    const int width = static_cast<int>(grid[0].size());
    if (width == 0) {
        return path;
    }

    auto in_bounds = [height, width](const int2& p) {
        return p.x >= 0 && p.y >= 0 && p.y < height && p.x < width;
    };

    auto is_passable = [&grid, &obstacles, &in_bounds](const int2& p) {
        if (!in_bounds(p)) return false;
        if (grid[p.y][p.x] != Dungeon::FLOOR) return false;
        return obstacles.find(p) == obstacles.end();
    };

    if (!is_passable(start) || !is_passable(goal)) {
        return path;
    }

    auto heuristic = [&goal](const int2& p) { return dist(p, goal); };

    std::priority_queue<AStarNode, std::vector<AStarNode>, AStarNodeCompare> open;

    std::vector<std::vector<bool>> closed(height, std::vector<bool>(width, false));
    std::vector<std::vector<int>> gScore(height, std::vector<int>(width, std::numeric_limits<int>::max()));
    std::vector<std::vector<int2>> cameFrom(height, std::vector<int2>(width, int2(-1, -1)));

    gScore[start.y][start.x] = 0;
    open.push(AStarNode{ start, 0, heuristic(start) });

    const int2 directions[4] = {
        int2{1, 0},
        int2{-1, 0},
        int2{0, 1},
        int2{0, -1}
    };

    while (!open.empty()) {
        AStarNode current = open.top();
        open.pop();

        const int2& cp = current.pos;

        if (closed[cp.y][cp.x]) {
            continue;
        }
        closed[cp.y][cp.x] = true;

        if (cp.x == goal.x && cp.y == goal.y) {
            // Reconstruct path
            int2 p = goal;
            while (!(p.x == start.x && p.y == start.y)) {
                path.push(p);
                const int2& prev = cameFrom[p.y][p.x];
                if (prev.x == -1 && prev.y == -1) {
                    // No path actually found
                    return std::stack<int2>();
                }
                p = prev;
            }
            path.push(start);
            return path;
        }

        for (const int2& dir : directions) {
            int2 neighbor{ cp.x + dir.x, cp.y + dir.y };
            if (!is_passable(neighbor)) {
                continue;
            }

            if (closed[neighbor.y][neighbor.x]) {
                continue;
            }

            int tentativeG = gScore[cp.y][cp.x] + 1;
            if (tentativeG < gScore[neighbor.y][neighbor.x]) {
                cameFrom[neighbor.y][neighbor.x] = cp;
                gScore[neighbor.y][neighbor.x] = tentativeG;
                int fScore = tentativeG + heuristic(neighbor);
                open.push(AStarNode{ neighbor, tentativeG, fScore });
            }
        }
    }

    // No path found
    return std::stack<int2>();
}