#pragma once

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <vector>

#include "common.h"

GLuint createTexture(std::vector<uint32_t>& framebuffer);
void updateTexture(std::vector<uint32_t>& framebuffer, GLuint tex);