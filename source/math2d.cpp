#include "math2d.h"


int dist(const int2 p, const int2 q) {
    return std::abs(p.x - q.x) + std::abs(p.y - q.y);
}

int norm(const int2 p) {
    return std::abs(p.x) + std::abs(p.y);
}

int2 int2::normalize() const {
    if (norm(*this) == 0)
        return {0, 0};
    if (std::abs(this->x) >= std::abs(this->y))
        return (this->x > 0) ? int2{1, 0} : int2{-1, 0};
    return (this->y > 0) ? int2{0, 1} : int2{0, -1};
}
