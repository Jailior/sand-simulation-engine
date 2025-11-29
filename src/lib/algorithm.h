#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include "common.h"

int idx(int x, int y);
bool in_bounds(int x, int y);
void update_sand(std::vector<particle>& grid, int x, int y);
void update_water(std::vector<particle>& grid, int x, int y);
void stepSimulation(std::vector<particle>& grid, std::vector<uint32_t>& framebuffer, uint32_t* palette);