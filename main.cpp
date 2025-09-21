// #include "lib/common.h"
#include "lib/texture_tools.h"


// Each pixel is 0xAARRGGBB (Alpha, Red, Green, Blue)
std::vector<uint32_t> framebuffer(WIDTH * HEIGHT, 0xFF000000);

enum Material { EMPTY, SAND, WATER, STONE };
uint32_t palette[] = {
    0xFF000000, // empty = black
    0xFFFFFF00, // sand = yellow
    0xFF0000FF, // water = blue
    0xFF888888  // stone = gray
};


// Example simulation step: random noise
void stepSimulation() {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (framebuffer[i] == palette[STONE]) continue; // stone is static
        uint mat = rand() % 4;
        framebuffer[i] = palette[mat];
    }
}


int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    // Create window with OpenGL context
    SDL_Window* window = SDL_CreateWindow("Sand Simulation Engine",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIN_W, WIN_H, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return -1;
    }
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << "\n";
        return -1;
    }

    // Setup OpenGL state
    glViewport(0, 0, WIN_W, WIN_H);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, -1, 1); // Normalized quad
    glMatrixMode(GL_MODELVIEW);

    GLuint tex = createTexture(framebuffer);

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        stepSimulation();
        updateTexture(framebuffer, tex);

        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex2f(0,0);
            glTexCoord2f(1,0); glVertex2f(1,0);
            glTexCoord2f(1,1); glVertex2f(1,1);
            glTexCoord2f(0,1); glVertex2f(0,1);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        SDL_GL_SwapWindow(window);
    }

    glDeleteTextures(1, &tex);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
