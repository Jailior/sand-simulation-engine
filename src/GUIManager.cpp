#include "lib/GUIManager.h"

GUIManager::GUIManager() : selectedHotbarSlot(0), digitFont({{
    {0x1F, 0x11, 0x1F}, // 0
    {0x00, 0x1F, 0x00}, // 1
    {0x1D, 0x15, 0x17}, // 2
    {0x15, 0x15, 0x1F}, // 3
    {0x07, 0x04, 0x1F}, // 4
    {0x17, 0x15, 0x1D}, // 5
    {0x1F, 0x15, 0x1D}, // 6
    {0x01, 0x01, 0x1F}, // 7
    {0x1F, 0x15, 0x1F}, // 8
    {0x17, 0x15, 0x1F}, // 9
}}) {
}

Material GUIManager::getSelectedMaterial() const {
    return hotbar[selectedHotbarSlot];
}

void GUIManager::setSelectedHotbarSlot(int newIndex) {
    if (newIndex >= 0 && newIndex < hotbarSlots) {
        selectedHotbarSlot = newIndex;
    } else {
        std::cerr << "Invalid hotbar slot index: " << newIndex << "\n";
    }
}

int GUIManager::idx(int x, int y) {
    return y * WIDTH + x;
}

void GUIManager::drawHotBar(std::vector<uint32_t>& frameBuffer, std::vector<uint32_t>* palettePtr, int y0) {
    const int bar_h = 36;
    const int slot_w = WIDTH / hotbarSlots;

    drawRect(frameBuffer, 0, y0, WIDTH, bar_h, DARK_GRAY);

    for (int i = 0; i < hotbarSlots; ++i) {
        int slot_x0 = i * slot_w;
        int slot_w_i = slot_w - 2;
        int slot_y0 = y0 + 1;
        
        Material m = hotbar[i];
        uint32_t fill = palettePtr->at(static_cast<uint8_t>(m));
        
        drawRect(frameBuffer, slot_x0 + 1, slot_y0, slot_w_i, bar_h - 4, fill);

        if (i == selectedHotbarSlot) {
            uint32_t border = WHITE;
            drawRect(frameBuffer, slot_x0 + 1, slot_y0, slot_w_i, 1, border); // top
            drawRect(frameBuffer, slot_x0 + 1, slot_y0 + (bar_h - 5), slot_w_i, 1, border); // bottom
            drawRect(frameBuffer, slot_x0 + 1, slot_y0, 1, bar_h - 4, border); // left
            drawRect(frameBuffer, slot_x0 + 1 + slot_w_i - 1, slot_y0, 1, bar_h - 4, border); // right
        }

        // draw slot number at bottom left of slot
        int num = i + 1;
        // compute contrasting color: if fill is bright, use black, else white
        uint32_t rgb = fill & 0x00FFFFFF;
        int r = (rgb >> 16) & 0xFF;
        int g = (rgb >> 8) & 0xFF;
        int b = (rgb) & 0xFF;
        int lum = (r*299 + g*587 + b*114) / 1000;
        uint32_t digit_color = (lum > 128) ? BLACK : WHITE;

        int dx = slot_x0 + 2 + (slot_w_i - 3) / 2;
        int dy = slot_y0 + (bar_h - 4 - 5) / 2;
        drawDigit(frameBuffer, dx, dy, num % 10, digit_color);
    }
}

void GUIManager::drawFPSCounter(std::vector<uint32_t>& frameBuffer, std::vector<uint32_t>* palettePtr, int x0, int y0, float fps) {
    // Draw FPS as integer
    int fpsInt = static_cast<int>(fps);
    uint32_t color = WHITE;
    // Draw each digit
    int hundreds = (fpsInt / 100) % 10;
    int tens = (fpsInt / 10) % 10;
    int units = fpsInt % 10;

    int spacing = 8; // space between digits
    if (hundreds > 0) {
        drawDigit(frameBuffer, x0, y0, hundreds, color);
        drawDigit(frameBuffer, x0 + 3 * 2 + spacing, y0, tens, color);
        drawDigit(frameBuffer, x0 + 2 * (3 * 2 + spacing), y0, units, color);
    } else if (tens > 0) {
        drawDigit(frameBuffer, x0, y0, tens, color);
        drawDigit(frameBuffer, x0 + 3 * 2 + spacing, y0, units, color);
    } else {
        drawDigit(frameBuffer, x0, y0, units, color);
    }
}

void GUIManager::drawDigit(std::vector<uint32_t>& frameBuffer, int x0, int y0, uint8_t digit, uint32_t color) {
    if (digit < 0 || digit > 9) {
        std::cerr << "Invalid digit to draw: " << static_cast<int>(digit) << "\n";
        return;
    }
    // font is 3 columns x 5 rows (use lower 5 bits of each byte)
    for (int cx = 0; cx < 3; ++cx) {
        uint8_t col = digitFont[digit][cx];
        for (int ry = 0; ry < 5; ++ry) {
            if ((col >> ry) & 1) {
                int scale = 2;
                int px = x0 + (cx * scale);
                int py = y0 + (ry * scale);
                // Draw a scale x scale block
                for (int dx = 0; dx < scale; ++dx) {
                    for (int dy = 0; dy < scale; ++dy) {
                        int final_px = px + dx;
                        int final_py = py + dy;
                        if (final_px >= 0 && final_px < WIDTH && final_py >= 0 && final_py < HEIGHT) {
                            frameBuffer[idx(final_px, final_py)] = color;
                        }
                    }
                }
            }
        }
    }
}

void GUIManager::drawRect(std::vector<uint32_t>& frameBuffer, int x0, int y0, int w, int h, uint32_t color) {
    for (int y = y0; y < y0 + h; ++y) {
        if (y < 0 || y >= HEIGHT) continue;
        for (int x = x0; x < x0 + w; ++x) {
            if (x < 0 || x >= WIDTH) continue;
            frameBuffer[idx(x,y)] = color;
        }
    }
}