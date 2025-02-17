#ifndef DRAW_HPP
#define DRAW_HPP

#include <vector>

#include "draw.hpp"
#include "utils.hpp"

void drawTriangles(const std::vector<Point>& points);

void drawFile(const char* filename, const std::vector<Point>& points);

void drawAxis();

#endif