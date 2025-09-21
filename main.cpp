// #include "lib/common.h"
#include "lib/texture_tools.h"

enum Material : uint8_t { EMPTY, SAND, WATER, STONE };

uint32_t palette[] = {
    0xFF000000, // empty = black
    0xFFFFFF00, // sand = yellow
    0xFF0000FF, // water = blue
    0xFF888888  // stone = gray
};


// Each pixel is 0xAARRGGBB (Alpha, Red, Green, Blue)
std::vector<uint32_t> framebuffer(WIDTH * HEIGHT, palette[EMPTY]);
std::vector<Material> grid(WIDTH * HEIGHT, EMPTY);

inline int idx(int x, int y) {
    return y * WIDTH + x;
}

inline bool in_bounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}




void stepSimulation() {
    // Iterate from bottom up so particles don't "fall twice"
    for (int y = HEIGHT - 2; y >= 0; --y) {
        for (int x = 0; x < WIDTH; ++x) {
            int i = idx(x, y);
            Material m = grid[i];
            if (m == EMPTY || m == STONE) continue;

            if (m == SAND) {
                // Try move down
                if (in_bounds(x, y+1) && grid[idx(x, y+1)] == EMPTY) {
                    std::swap(grid[i], grid[idx(x, y+1)]);
                }
                // Try diagonals
                else if (in_bounds(x-1, y+1) && grid[idx(x-1, y+1)] == EMPTY) {
                    std::swap(grid[i], grid[idx(x-1, y+1)]);
                }
                else if (in_bounds(x+1, y+1) && grid[idx(x+1, y+1)] == EMPTY) {
                    std::swap(grid[i], grid[idx(x+1, y+1)]);
                }
            }
            else if (m == WATER) {
                if (in_bounds(x, y+1) && grid[idx(x, y+1)] == EMPTY) {
                    std::swap(grid[i], grid[idx(x, y+1)]);
                }
                else {
                    int dir = (rand() & 1) ? -1 : 1; // left or right
                    if (in_bounds(x+dir, y) && grid[idx(x+dir, y)] == EMPTY) {
                        std::swap(grid[i], grid[idx(x+dir, y)]);
                    }
                }
            }
        }
    }

    // Update framebuffer colors
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        framebuffer[i] = palette[grid[i]];
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

        int mx, my;
        Uint32 mouse = SDL_GetMouseState(&mx, &my);
        if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            int gx = mx * WIDTH / WIN_W;
            int gy = my * HEIGHT / WIN_H;
            if (in_bounds(gx, gy)) grid[idx(gx, gy)] = SAND;
        }
        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            int gx = mx * WIDTH / WIN_W;
            int gy = my * HEIGHT / WIN_H;
            if (in_bounds(gx, gy)) grid[idx(gx, gy)] = WATER;
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
