#include "lib/RenderClient.h"

RenderClient::RenderClient(std::vector<uint32_t>* frameBuffer)
    : frameBuffer(frameBuffer) {}

void RenderClient::initializeWindow(const char* title, int width, int height, SDL_Window*& outWindow, SDL_GLContext& outContext) {
    outWindow = SDL_CreateWindow(title,
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 width, height, SDL_WINDOW_OPENGL);
    if (!outWindow) {
        throw std::runtime_error("Failed to create SDL window" + std::string(SDL_GetError()));
    }

    outContext = SDL_GL_CreateContext(outWindow);
    if (!outContext) {
        SDL_DestroyWindow(outWindow);
        throw std::runtime_error("Failed to create OpenGL context" + std::string(SDL_GetError()));
    }
}

void RenderClient::setUpGLState(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, -1, 1); // Normalized quad
    glMatrixMode(GL_MODELVIEW);
}

bool RenderClient::isExitRequested() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return true;
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) return true;
    }
    return false;
}

bool RenderClient::isMouseLeftPressed(int& gx, int& gy) {
    int mx, my;
    Uint32 mouse = SDL_GetMouseState(&mx, &my);
    if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        gx = mx * WIDTH / WIN_W;
        gy = my * HEIGHT / WIN_H;
        return true;
    }
    return false;
}

bool RenderClient::isMouseRightPressed(int& gx, int& gy) {
    int mx, my;
    Uint32 mouse = SDL_GetMouseState(&mx, &my);
    if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        gx = mx * WIDTH / WIN_W;
        gy = my * HEIGHT / WIN_H;
        return true;
    }
    return false;
}

GLuint RenderClient::initializeTexture() {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, (*frameBuffer).data());
    return tex;
}

void RenderClient::updateTexture(GLuint textureID) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT,
                    GL_BGRA, GL_UNSIGNED_BYTE, (*frameBuffer).data());
}

void RenderClient::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderClient::renderFrame(SDL_Window* window, GLuint textureID) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(0,0);
        glTexCoord2f(1,0); glVertex2f(1,0);
        glTexCoord2f(1,1); glVertex2f(1,1);
        glTexCoord2f(0,1); glVertex2f(0,1);
    glEnd();

    SDL_GL_SwapWindow(window);
}

void RenderClient::cleanup(SDL_Window* window, SDL_GLContext context, GLuint textureID) {
    glDeleteTextures(1, &textureID);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
}