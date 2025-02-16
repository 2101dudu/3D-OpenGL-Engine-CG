#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include <vector>

#include "draw.hpp"
#include "utils.hpp"

#define MODELS "../models/"

void drawTriangles(const std::vector<Point>& points) {
    glBegin(GL_TRIANGLES);
    for (const auto& point : points) {
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
}

void drawFile(const char* filename) {
    std::vector<Point> points = parseFile(filename);
    if (points.empty()) {
        // File not found or empty, handle this case
        return;
    }
    drawTriangles(points);
}