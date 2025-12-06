#pragma once

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <vector>
#include <iostream>
#include <stdexcept>

#include "common.h"

class RenderClient {
    private:
        // Each pixel is 0xAARRGGBB (Alpha, Red, Green, Blue)
        std::vector<uint32_t>* frameBuffer;
        
    public:
        RenderClient(std::vector<uint32_t>* frameBuffer);

        void initializeWindow(const char* title, int width, int height, SDL_Window*& outWindow, SDL_GLContext& outContext);
        void setUpGLState(int width, int height);
        bool isExitRequested();
        bool isMouseLeftPressed(int& gx, int& gy);
        bool isMouseRightPressed(int& gx, int& gy);
        GLuint initializeTexture();
        void updateTexture(GLuint textureID);
        void clear();
        void renderFrame(SDL_Window* window, GLuint textureID);
        void cleanup(SDL_Window* window, SDL_GLContext context, GLuint textureID);
};