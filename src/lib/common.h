#pragma once

#include <GL/gl.h>

// Grid resolution
constexpr int WIDTH =   320;
constexpr int HEIGHT =  240;

// Window resolution (can scale up)
constexpr int WIN_W = 960;
constexpr int WIN_H = 720;

constexpr uint32_t BLACK =    0xFF000000;
constexpr uint32_t YELLOW =   0xFFFFFF00;
constexpr uint32_t BLUE =     0xFF0000FF;
constexpr uint32_t GRAY =     0xFF888888;

enum class Material : uint8_t { EMPTY, SAND, WATER, STONE };

struct Particle {
    int x;
    int y;
    Material type;
};