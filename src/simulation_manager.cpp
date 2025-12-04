#include "lib/simulation_manager.h"

class SimulationManager {
    private:
        // Each pixel is 0xAARRGGBB (Alpha, Red, Green, Blue)
        std::vector<uint32_t> framebuffer(WIDTH * HEIGHT, palette[Material::EMPTY]);
        std::vector<particle> grid(WIDTH * HEIGHT, {0,0,Material::EMPTY});
}