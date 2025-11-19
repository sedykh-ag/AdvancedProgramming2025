#pragma once

#include <vector>

#include "health.h"
#include "sprite.h"
#include "stamina.h"
#include "transform2d.h"


struct Camera {
    float pixelsPerMeter{32.0f};
    Transform2D transform{0.0, 0.0};
};

struct Cells {
    std::vector<Sprite> sprites;
    std::vector<Transform2D> transforms;
};

struct Characters {
    std::vector<Sprite> sprites;
    std::vector<Transform2D> transforms;
    std::vector<Health> healths;
    std::vector<Stamina> staminas;
    std::vector<float> timeSinceLastMove;

    std::vector<uint8_t> isHero;
    std::vector<uint8_t> isPredator;
};
