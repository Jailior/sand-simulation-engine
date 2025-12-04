#pragma once

#include <GL/gl.h>

// Grid resolution
constexpr int WIDTH = 320;
constexpr int HEIGHT = 240;

// Window resolution (can scale up)
constexpr int WIN_W = 960;
constexpr int WIN_H = 720;

enum class Material : uint8_t { EMPTY, SAND, WATER, STONE };

struct particle {
    int x;
    int y;
    Material type;
};