#include "lib/algorithm.h"

int idx(int x, int y) {
    return y * WIDTH + x;
}

bool in_bounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

inline void update_sand(std::vector<particle>& grid, int x, int y) {
    int i = idx(x, y);
    // Try move down
    if (in_bounds(x, y+1) && grid[idx(x, y+1)].type == Material::EMPTY) {
        std::swap(grid[i], grid[idx(x, y+1)]);
    }
    // Try diagonals
    else if (in_bounds(x-1, y+1) && grid[idx(x-1, y+1)].type == Material::EMPTY) {
        std::swap(grid[i], grid[idx(x-1, y+1)]);
    }
    else if (in_bounds(x+1, y+1) && grid[idx(x+1, y+1)].type == Material::EMPTY) {
        std::swap(grid[i], grid[idx(x+1, y+1)]);
    }
}

inline void update_water(std::vector<particle>& grid, int x, int y) {
    int i = idx(x, y);
    if (in_bounds(x, y+1) && grid[idx(x, y+1)].type == Material::EMPTY) {
        std::swap(grid[i], grid[idx(x, y+1)]);
    }
    else {
        int dir = (rand() & 1) ? -1 : 1; // left or right
        if (in_bounds(x+dir, y) && grid[idx(x+dir, y)].type == Material::EMPTY) {
            std::swap(grid[i], grid[idx(x+dir, y)]);
        }
    }
}


void stepSimulation(std::vector<particle>& grid, std::vector<uint32_t>& framebuffer, uint32_t* palette) {
    // Iterate from bottom up so particles don't "fall twice"
    for (int y = HEIGHT - 2; y >= 0; --y) {
        for (int x = 0; x < WIDTH; ++x) {
            Material m = grid[idx(x, y)].type;
            if (m == Material::EMPTY || m == Material::STONE) continue;

            if (m == Material::SAND) {
                update_sand(grid, x, y);
            }
            else if (m == Material::WATER) {
                update_water(grid, x, y);
            }
        }
    }

    // Update framebuffer colors
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        framebuffer[i] = palette[grid[i].type];
    }
}