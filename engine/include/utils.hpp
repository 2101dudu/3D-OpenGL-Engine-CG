#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

struct Point {
    float x, y, z;

    Point(float x_val = 0.0f, float y_val = 0.0f, float z_val = 0.0f)
        : x(x_val), y(y_val), z(z_val) {}
};

struct Association {
    int p1, p2, p3;
};

std::vector<Point> parseFile(const std::string& filename);

#endif