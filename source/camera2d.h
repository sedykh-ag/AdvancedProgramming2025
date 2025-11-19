#pragma once

#include "transform2d.h"
#include <SDL3/SDL_rect.h>

struct Camera {
    float pixelsPerMeter{32.0f};
    Transform2D transform{0.0, 0.0};
};

inline SDL_FRect to_camera_space(const Transform2D& object_transform, const Transform2D& camera_transform, const float pixelsPerMeter) {
    float camX = (object_transform.x - camera_transform.x) * pixelsPerMeter;
    float camY = (object_transform.y - camera_transform.y) * pixelsPerMeter;
    float camW = object_transform.sizeX * pixelsPerMeter;
    float camH = object_transform.sizeY * pixelsPerMeter;
    return SDL_FRect{camX, camY, camW, camH};
}