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
    if (type == Material::EMPTY || type == Material::STONE) {
        setMaterialSprayAt(x, y, type, 5, 100, false);
    } else {
        setMaterialSprayAt(x, y, type, 15, 2, true);
    }
    return true;
}

// Sets material in a spray pattern with given radius and density
// Density is percentage chance (0-100) to place a particle at each position
void SimulationManager::setMaterialSprayAt(int x, int y, Material type, int radius, int density, bool randomOffset) {
    for (int by = -radius; by <= radius; by++) {
            for (int bx = -radius; bx <= radius; bx++) {
                int nx = x + bx;
                int ny = y + by;
                if (randomOffset) {
                     int offset = (rand() % 3) - 1; // -1, 0, or 1
                     nx += offset;
                     ny += offset;
                }
                bool randomCheck = (rand() % 100) < density;
                if (!randomCheck) continue;
                if (inBounds(nx, ny) && (grid[idx(nx, ny)].type == Material::EMPTY || type == Material::EMPTY)) {
                    grid[idx(nx, ny)].type = type;
                    grid[idx(nx, ny)].density = densityMap[static_cast<int>(type)];
                }
        }
    }
}

SimulationManager::SimulationManager() {
    // Seed random number generator
    std::srand(std::time(nullptr));
    
    palette = {
        BLACK,    // empty = black
        YELLOW,   // sand = yellow
        BLUE,     // water = blue
        GRAY      // stone = gray
    };

    densityMap = {
        EMPTY_DENSITY,
        SAND_DENSITY,
        WATER_DENSITY,
        STONE_DENSITY
    };

    frameBuffer.resize(WIDTH * HEIGHT, palette[static_cast<int>(Material::EMPTY)]);
    grid.resize(WIDTH * HEIGHT, {0,0,Material::EMPTY,EMPTY_DENSITY});
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
    if (inBounds(x, y+1) && grid[idx(x, y+1)].density < grid[i].density) {
        std::swap(grid[i], grid[idx(x, y+1)]);
    }
    // Try diagonals
    bool canMoveLeftDiagonal = inBounds(x-1, y+1) && grid[idx(x-1, y+1)].density < grid[i].density;
    bool canMoveRightDiagonal = inBounds(x+1, y+1) && grid[idx(x+1, y+1)].density < grid[i].density;
    if (canMoveLeftDiagonal && canMoveRightDiagonal) {
        if (rand() & 1) {
            std::swap(grid[i], grid[idx(x-1, y+1)]);
        }
        else {
            std::swap(grid[i], grid[idx(x+1, y+1)]);
        }
    }
    else if (canMoveLeftDiagonal) {
        std::swap(grid[i], grid[idx(x-1, y+1)]);
    }
    else if (canMoveRightDiagonal) {
        std::swap(grid[i], grid[idx(x+1, y+1)]);
    }
}

inline void SimulationManager::updateWater(std::vector<Particle>& grid, int x, int y) {
    int i = idx(x, y);
    if (inBounds(x, y+1) && grid[idx(x, y+1)].density < grid[i].density) {
            std::swap(grid[i], grid[idx(x, y+1)]);
    }
    // Try diagonals
    bool canMoveLeftDiagonal = inBounds(x-1, y+1) && grid[idx(x-1, y+1)].density < grid[i].density;
    bool canMoveRightDiagonal = inBounds(x+1, y+1) && grid[idx(x+1, y+1)].density < grid[i].density;
    if (canMoveLeftDiagonal && canMoveRightDiagonal) {
        if (rand() & 1) {
            std::swap(grid[i], grid[idx(x-1, y+1)]);
        }
        else {
            std::swap(grid[i], grid[idx(x+1, y+1)]);
        }
    }
    else if (canMoveLeftDiagonal) {
        std::swap(grid[i], grid[idx(x-1, y+1)]);
    }
    else if (canMoveRightDiagonal) {
        std::swap(grid[i], grid[idx(x+1, y+1)]);
    }
    else {
        int dir = (rand() & 1) ? -1 : 1; // left or right
        if (inBounds(x+dir, y) && grid[idx(x+dir, y)].density < grid[i].density) {
            std::swap(grid[i], grid[idx(x+dir, y)]);
        }
    }
}