#pragma once

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <vector>
#include <array>
#include <iostream>
#include <stdexcept>

#include "common.h"

class GUIManager {
    // THE GUI MANAGER HANDLES THE HOTBAR STATE AND
    // UPDATING RELEVANT FIELDS AND SENDING EVENTS
    public:
        std::array<std::array<uint8_t, 3>, 10> digitFont;
        static constexpr int hotbarSlots = 5;
        GUIManager();

        Material getSelectedMaterial() const;
        void setSelectedHotbarSlot(int newIndex);
        void drawHotBar(std::vector<uint32_t>& frameBuffer, std::vector<uint32_t>* palettePtr, int y0);
        void drawFPSCounter(std::vector<uint32_t>& frameBuffer, std::vector<uint32_t>* palettePtr, int x0, int y0, float fps);

    private:
        int selectedHotbarSlot;
        std::array<Material, hotbarSlots> hotbar = {
            Material::EMPTY,
            Material::SAND,
            Material::WATER,
            Material::STONE,
            Material::EMPTY,
        };

        int idx(int x, int y);
        void drawDigit(std::vector<uint32_t>& frameBuffer, int x0, int y0, uint8_t digit, uint32_t color);
        void drawRect(std::vector<uint32_t>& frameBuffer, int x0, int y0, int w, int h, uint32_t color);    
};