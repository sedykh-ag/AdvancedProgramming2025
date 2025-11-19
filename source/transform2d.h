#pragma once

struct Transform2D {
    using value_type = double;
    value_type x, y;
    value_type sizeX, sizeY;
    Transform2D(value_type x=0, value_type y=0, value_type sizeX=1, value_type sizeY=1)
        : x(x), y(y), sizeX(sizeX), sizeY(sizeY) {}

};

static_assert(sizeof(Transform2D) == sizeof(Transform2D::value_type) * 4, "I don't know why Transform2D isn't just 4 value_types ???");