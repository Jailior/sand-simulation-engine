#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <vector>
#include <iostream>
#include <chrono>

#include "lib/common.h"
#include "lib/GUIManager.h"
#include "lib/SimulationManager.h"
#include "lib/RenderClient.h"

bool RUNNING = true;

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    auto& simManager = SimulationManager::getInstance();
    auto& framebuffer = simManager.getFrameBuffer();
    auto* palettePtr =  &simManager.getPalette();

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

    GUIManager guiManager = GUIManager();

    renderClient.setUpGLState(WIN_W, WIN_H);
    GLuint textureID = renderClient.initializeTexture();

    auto start = std::chrono::steady_clock::now();
    int nbFrames = 0;
    int fps_int = 0;

    while (RUNNING) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (renderClient.isExitRequested(e)) {
                RUNNING = false;
                break;
            }
            int hotbarInput = renderClient.getHotbarInput(e, guiManager.hotbarSlots);
            if (hotbarInput != -1) {
                guiManager.setSelectedHotbarSlot(hotbarInput);
            }
        }


        Material selectedMat = guiManager.getSelectedMaterial();
        int gx, gy;
        if (renderClient.isMouseLeftPressed(gx, gy)) {
            simManager.setMaterialAt(gx, gy, selectedMat);
        }
        if (renderClient.isMouseRightPressed(gx, gy)) {
            simManager.setMaterialAt(gx, gy, Material::WATER);
        }

        simManager.stepSimulation();
        guiManager.drawHotBar(framebuffer, palettePtr, 0);

        auto end = std::chrono::steady_clock::now();
        nbFrames++;
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (elapsed >= FPS_INTERVAL_MS) {
            float fps = nbFrames * 1000.0f / elapsed;
            fps_int = static_cast<int>(fps);
            nbFrames = 0;
            start = end;
        }

        if (fps_int > 0) {
            guiManager.drawFPSCounter(framebuffer, palettePtr, 10, HEIGHT - 20, fps_int);
        }

        renderClient.updateTexture(textureID);
        renderClient.clear();
        renderClient.renderFrame(window, textureID);
    }

    renderClient.cleanup(window, glContext, textureID);
    SDL_Quit();
    return 0;
}
