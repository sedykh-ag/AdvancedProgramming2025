#pragma once

#include <functional>


struct int2 {
    int x, y;
    int2(int x=0, int y=0) : x(x), y(y) {}

    bool operator==(const int2& other) const {
        return x == other.x && y == other.y;
    }

    int2 operator-(const int2& other) const {
        return int2{x - other.x, y - other.y};
    }

    int2 operator+(const int2& other) const {
        return int2{x + other.x, y + other.y};
    }

    int2 normalize() const;
};

namespace std {
    template<>
    struct hash<int2> {
        size_t operator()(const int2& v) const noexcept {
            // Simple combination of coordinates into a 64-bit value
            // assuming int is 32-bit on the target platform.
            return (static_cast<size_t>(static_cast<unsigned int>(v.x)) << 32) ^
                   static_cast<size_t>(static_cast<unsigned int>(v.y));
        }
    };
}

struct float2 {
    float x, y;
    float2(float x=0, float y=0) : x(x), y(y) {}
};

int dist(const int2 p, const int2 q);
int norm(const int2 p);
