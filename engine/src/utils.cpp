#include "utils.hpp"
#include <fstream>
#include <iostream>

std::vector<Point> parseFile(const std::string& filename) {
    std::vector<Point> points;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return points;
    }

    size_t numPoints, numAssociations;
    file >> numPoints;
    points.resize(numPoints);
    for (auto& point : points) {
        file >> point.x >> point.y >> point.z;
    }

    file >> numAssociations;
    // Skip associations for now
    for (size_t i = 0; i < numAssociations; ++i) {
        int p1, p2, p3;
        file >> p1 >> p2 >> p3;
    }

    file.close();
    return points;
}