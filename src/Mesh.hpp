#pragma once
#include <glad/glad.h>
#include <span>


class Mesh{
public:
    Mesh(std::span<const float> vertices, std::span<const unsigned int> indices);
    void render();

private:
    GLuint VAO = 0;
    unsigned int idx_count = 0;
};
    