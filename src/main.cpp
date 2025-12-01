#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <vector>
#include <array>
#include <cstdint>
#include <iostream>

#include "lib/texture_tools.h"
#include "lib/algorithm.h"
#include "lib/common.h"

uint32_t palette[] = {
    0xFF000000, // empty = black
    0xFFFFFF00, // sand = yellow
    0xFF0000FF, // water = blue
    0xFF888888  // stone = gray
};


// Each pixel is 0xAARRGGBB (Alpha, Red, Green, Blue)
std::vector<uint32_t> framebuffer(WIDTH * HEIGHT, palette[EMPTY]);
std::vector<particle> grid(WIDTH * HEIGHT, {0,0,EMPTY});

// Hotbar configuration
constexpr int HOTBAR_SLOTS = 5;
// Slot -> Material mapping (1-based slots for user). We'll set some defaults so '2' selects SAND.
// static std::array<Material, HOTBAR_SLOTS> hotbar = { EMPTY, SAND, WATER, STONE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY };
static std::array<Material, HOTBAR_SLOTS> hotbar = { EMPTY, SAND, WATER, STONE, EMPTY };
static int selected_slot = 1; // 1-based index (user-visible). Default to slot 1 (EMPTY)

// Small 3x5 pixel font for digits '0'..'9' (each int uses lowest 5 bits per column)
static const uint8_t digit_font[10][3] = {
    {0x1F, 0x11, 0x1F}, // 0
    {0x00, 0x1F, 0x00}, // 1 (vertical middle)
    {0x1D, 0x15, 0x17}, // 2
    {0x15, 0x15, 0x1F}, // 3
    {0x07, 0x04, 0x1F}, // 4
    {0x17, 0x15, 0x1D}, // 5
    {0x1F, 0x15, 0x1D}, // 6
    {0x01, 0x01, 0x1F}, // 7
    {0x1F, 0x15, 0x1F}, // 8
    {0x17, 0x15, 0x1F}, // 9
};

// draw a filled rectangle into the framebuffer
static void drawRect(std::vector<uint32_t>& fb, int x0, int y0, int w, int h, uint32_t color){
    for (int y = y0; y < y0 + h; ++y) {
        if (y < 0 || y >= HEIGHT) continue;
        for (int x = x0; x < x0 + w; ++x) {
            if (x < 0 || x >= WIDTH) continue;
            fb[idx(x,y)] = color;
        }
    }
}

// draw a single digit (1..9) using the small font in slot-local coordinates
static void drawDigit(std::vector<uint32_t>& fb, int x0, int y0, int digit, uint32_t color){
    if (digit < 0 || digit > 9) return;
    // font is 3 columns x 5 rows (use lower 5 bits of each byte)
    for (int cx = 0; cx < 3; ++cx) {
        uint8_t col = digit_font[digit][cx];
        for (int ry = 0; ry < 5; ++ry) {
            if ((col >> ry) & 1) {
                int px = x0 + cx;
                int py = y0 + ry;
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    fb[idx(px, py)] = color;
                }
            }
        }
    }
}

// Draw hotbar overlay at bottom of the framebuffer
static void drawHotbar(std::vector<uint32_t>& fb, uint32_t* palettePtr){
    const int bar_h = 18; // rows
    const int slot_w = WIDTH / HOTBAR_SLOTS;
    // const int y0 = HEIGHT - bar_h - 1;
    const int y0 = 0;    

    // semi-transparent background (we'll use a dark gray)
    uint32_t bg = 0xFF202020; // dark gray
    drawRect(fb, 0, y0, WIDTH, bar_h, bg);

    for (int s = 0; s < HOTBAR_SLOTS; ++s) {
        int sx = s * slot_w;
        int sw = slot_w - 2; // leave 1px gap
        int sy = y0 + 1;

        Material m = hotbar[s];
        uint32_t fill = palettePtr[m];

        // Draw slot fill
        drawRect(fb, sx + 1, sy, sw, bar_h - 4, fill);

        // Draw border if selected
        if (s + 1 == selected_slot) {
            // Draw a 1px border in white
            uint32_t border = 0xFFFFFFFF;
            // top
            drawRect(fb, sx + 1, sy, sw, 1, border);
            // bottom
            drawRect(fb, sx + 1, sy + (bar_h - 5), sw, 1, border);
            // left
            drawRect(fb, sx + 1, sy, 1, bar_h - 4, border);
            // right
            drawRect(fb, sx + 1 + sw - 1, sy, 1, bar_h - 4, border);
        }

        // draw slot number near bottom-left of slot using the tiny font, scaled by 1 (3x5)
        int num = s + 1;
        // compute contrasting color: if fill is bright, use black, else white
        uint32_t rgb = fill & 0x00FFFFFF;
        int r = (rgb >> 16) & 0xFF;
        int g = (rgb >> 8) & 0xFF;
        int b = (rgb) & 0xFF;
        int lum = (r*299 + g*587 + b*114) / 1000;
        uint32_t digit_color = (lum > 128) ? 0xFF000000 : 0xFFFFFFFF;

        // center the 3x5 digit inside slot
        int dx = sx + 2 + (sw - 3) / 2;
        int dy = sy + (bar_h - 4 - 5) / 2;
        drawDigit(fb, dx, dy, num % 10, digit_color);
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
            if (e.type == SDL_KEYDOWN) {
                // Escape to quit
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                // Number keys 1..9 select hotbar slot
                else if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_9) {
                    int slot = (e.key.keysym.sym - SDLK_1) + 1; // 1-based
                    if (slot >= 1 && slot <= HOTBAR_SLOTS) selected_slot = slot;
                }
            }
        }

        int mx, my;
        Uint32 mouse = SDL_GetMouseState(&mx, &my);
        int gx = mx * WIDTH / WIN_W;
        int gy = my * HEIGHT / WIN_H;
        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            if (in_bounds(gx, gy)) {
                Material m = hotbar[selected_slot - 1];
                grid[idx(gx, gy)].type = m;
            }
        }
        if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            // Right-click to erase
            if (in_bounds(gx, gy)) grid[idx(gx, gy)].type = EMPTY;
        }

        stepSimulation(grid, framebuffer, palette);

        // Draw the hotbar overlay into the framebuffer and then upload the texture
        drawHotbar(framebuffer, palette);
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
