#include "draw.hpp"
#include "utils.hpp"
#include <GL/glut.h>
#include <vector>

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

    // Imprima os pontos no terminal
    printPoints(points);

    drawTriangles(points);
}