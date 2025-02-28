#ifndef DRAW_HPP
#define DRAW_HPP

#include "vector"

void drawAxis();

void drawWithoutVBOs(std::vector<std::vector<float>> modelsPoints);

void drawWithVBOs(std::vector<GLuint> buffers, std::vector<GLuint> verticesCount);

#endif
