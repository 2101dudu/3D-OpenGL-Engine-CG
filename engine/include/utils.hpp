#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

struct Point {
    float x, y, z;
};

struct Association {
    int p1, p2, p3;
};

std::vector<Point> parseFile(const std::string& filename);

void printPoints(const std::vector<Point>& points);

#endif