#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <vector>
#include <iostream>

#include "lib/SimulationManager.h"
#include "lib/RenderClient.h"
#include "lib/common.h"

bool RUNNING = true;

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    auto& simManager = SimulationManager::getInstance();
    auto& framebuffer = simManager.getFrameBuffer();

    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    RenderClient renderClient(&framebuffer);

    try {
        renderClient.initializeWindow("Sand Simulation Engine", WIN_W, WIN_H, window, glContext);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        SDL_Quit();
        return -1;
    }

    renderClient.setUpGLState(WIN_W, WIN_H);
    GLuint textureID = renderClient.initializeTexture();

    while (RUNNING) {
        if (renderClient.isExitRequested()) {
            RUNNING = false;
            continue;
        }

        int gx, gy;
        if (renderClient.isMouseLeftPressed(gx, gy)) {
            simManager.setMaterialAt(gx, gy, Material::WATER);
        }
        if (renderClient.isMouseRightPressed(gx, gy)) {
            simManager.setMaterialAt(gx, gy, Material::SAND);
        }

        simManager.stepSimulation();

        renderClient.updateTexture(textureID);
        renderClient.clear();
        renderClient.renderFrame(window, textureID);
    }

    renderClient.cleanup(window, glContext, textureID);
    SDL_Quit();
    return 0;
}
