#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include "common.h"

class SimulationManager {
    public:
        static SimulationManager& getInstance();

        SimulationManager(const SimulationManager&) = delete;
        SimulationManager& operator=(const SimulationManager&) = delete;

        std::vector<uint32_t>& getPalette();
        std::vector<uint32_t>& getFrameBuffer();
        std::vector<Particle>& getGrid();

        void stepSimulation();
        bool setMaterialAt(int x, int y, Material type);
    
    private:
        // Each pixel is 0xAARRGGBB (Alpha, Red, Green, Blue)
        std::vector<uint32_t> frameBuffer;
        std::vector<Particle> grid;
        std::vector<uint32_t> palette;
        std::vector<int> densityMap;
        
        SimulationManager();

        SimulationManager(SimulationManager&&) = delete;
        SimulationManager& operator=(SimulationManager&&) = delete;
        
        int idx(int x, int y);
        bool inBounds(int x, int y);
        
        inline void updateSand(std::vector<Particle>& grid, int x, int y);
        inline void updateWater(std::vector<Particle>& grid, int x, int y);
        void setMaterialSprayAt(int x, int y, Material type, int radius, int density, bool randomOffset);
};