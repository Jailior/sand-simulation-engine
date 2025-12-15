#pragma once

#include <GL/gl.h>

// Grid resolution
constexpr int WIDTH =   640;
constexpr int HEIGHT =  480;

// Window resolution (can scale up)
constexpr int WIN_W = 960;
constexpr int WIN_H = 720;

constexpr uint32_t BLACK =      0xFF000000;
constexpr uint32_t WHITE =      0xFFFFFFFF;
constexpr uint32_t YELLOW =     0xFF966432;
constexpr uint32_t BLUE =       0xC81464AA;
constexpr uint32_t GRAY =       0xFF888888;
constexpr uint32_t DARK_GRAY =  0xFF202020;

constexpr int EMPTY_DENSITY = 0;
constexpr int SAND_DENSITY = 70; // out of 100
constexpr int WATER_DENSITY = 30; // out of 100
constexpr int STONE_DENSITY = 100;

enum class Material : uint8_t { EMPTY, SAND, WATER, STONE };

struct Particle {
    int x;
    int y;
    Material type;
    int density;
};