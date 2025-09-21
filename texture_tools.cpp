#include "lib/texture_tools.h"

// Uploads framebuffer to OpenGL texture
GLuint createTexture(std::vector<uint32_t>& framebuffer) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.data());
    return tex;
}

void updateTexture(std::vector<uint32_t>& framebuffer, GLuint tex) {
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT,
                    GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.data());
}