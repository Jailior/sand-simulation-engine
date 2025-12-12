#include "lib/SimulationManager.h"

SimulationManager& SimulationManager::getInstance() {
    static SimulationManager instance;
    return instance;
}

std::vector<uint32_t>& SimulationManager::getPalette() {
    return palette;
}

std::vector<uint32_t>& SimulationManager::getFrameBuffer() {
    return frameBuffer;
}

std::vector<Particle>& SimulationManager::getGrid() {
    return grid;
}

void SimulationManager::stepSimulation() {
    // Iterate from bottom up so Particles don't "fall twice"
    for (int y = HEIGHT - 2; y >= 0; --y) {
        for (int x = 0; x < WIDTH; ++x) {
            Material m = grid[idx(x, y)].type;
            if (m == Material::EMPTY || m == Material::STONE) continue;

            if (m == Material::SAND) {
                updateSand(grid, x, y);
            }
            else if (m == Material::WATER) {
                updateWater(grid, x, y);
            }
        }
    }

    // Update frameBuffer colors
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        frameBuffer[i] = palette[static_cast<int>(grid[i].type)];
    }
}

bool SimulationManager::setMaterialAt(int x, int y, Material type) {
    if (!inBounds(x, y)) return false;
    if (type == Material::EMPTY) {
        grid[idx(x, y)].type = type;
    } else if (type == Material::STONE) {
       for (int by = -7; by <= 7; by++) {
            for (int bx = -7; bx <= 7; bx++) {
                int nx = x + bx;
                int ny = y + by;
                if (inBounds(nx, ny)) {
                    grid[idx(nx, ny)].type = type;
                }
            }
        }
    } 
    else {
        // place a brush of 20x20
        for (int by = -15; by <= 15; by++) {
            for (int bx = -15; bx <= 15; bx++) {
                int nx = x + bx;;
                int ny = y + by;
                int randomOffset = (rand() % 3) - 1; // -1, 0, or 1
                nx += randomOffset;
                ny += randomOffset;
                bool randomCheck = (rand() % 100) < 2; // 40% chance to place
                if (!randomCheck) continue;
                if (inBounds(nx, ny) && grid[idx(nx, ny)].type == Material::EMPTY) {
                    grid[idx(nx, ny)].type = type;
                }
            }
        }
    }
    return true;
}

SimulationManager::SimulationManager() {
        palette = {
            BLACK,    // empty = black
            YELLOW,   // sand = yellow
            BLUE,     // water = blue
            GRAY      // stone = gray
        };
    
        frameBuffer.resize(WIDTH * HEIGHT, palette[static_cast<int>(Material::EMPTY)]);
        grid.resize(WIDTH * HEIGHT, {0,0,Material::EMPTY});
}
                
int SimulationManager::idx(int x, int y) {
    return y * WIDTH + x;
}

bool SimulationManager::inBounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

inline void SimulationManager::updateSand(std::vector<Particle>& grid, int x, int y) {
    int i = idx(x, y);
    // Try move down
    if (inBounds(x, y+1) && grid[idx(x, y+1)].type == Material::EMPTY) {
        std::swap(grid[i], grid[idx(x, y+1)]);
    }
    // Try diagonals
    else if (inBounds(x-1, y+1) && grid[idx(x-1, y+1)].type == Material::EMPTY) {
        std::swap(grid[i], grid[idx(x-1, y+1)]);
    }
    else if (inBounds(x+1, y+1) && grid[idx(x+1, y+1)].type == Material::EMPTY) {
        std::swap(grid[i], grid[idx(x+1, y+1)]);
    }
}

inline void SimulationManager::updateWater(std::vector<Particle>& grid, int x, int y) {
    int i = idx(x, y);
    if (inBounds(x, y+1) && grid[idx(x, y+1)].type == Material::EMPTY) {
        std::swap(grid[i], grid[idx(x, y+1)]);
    }
    // Try diagonals
    else if (inBounds(x-1, y+1) && grid[idx(x-1, y+1)].type == Material::EMPTY) {
        std::swap(grid[i], grid[idx(x-1, y+1)]);
    }
    else if (inBounds(x+1, y+1) && grid[idx(x+1, y+1)].type == Material::EMPTY) {
        std::swap(grid[i], grid[idx(x+1, y+1)]);
    }
    else {
        int dir = (rand() & 1) ? -1 : 1; // left or right
        if (inBounds(x+dir, y) && grid[idx(x+dir, y)].type == Material::EMPTY) {
            std::swap(grid[i], grid[idx(x+dir, y)]);
        }
    }
}