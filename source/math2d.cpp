#include "math2d.h"


int dist(const int2 p, const int2 q) {
    return std::abs(p.x - q.x) + std::abs(p.y - q.y);
}
