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
    std::vector<Point> allPoints(numPoints);
    for (auto& point : allPoints) {
        file >> point.x >> point.y >> point.z;
    }

    file >> numAssociations;
    std::vector<Association> associations(numAssociations);
    for (auto& assoc : associations) {
        file >> assoc.p1 >> assoc.p2 >> assoc.p3;
    }

    for (const auto& assoc : associations) {
        points.push_back(allPoints[assoc.p1 - 1]);
        points.push_back(allPoints[assoc.p2 - 1]);
        points.push_back(allPoints[assoc.p3 - 1]);
    }

    file.close();
    return points;
}

void printPoints(const std::vector<Point>& points) {
    for (const auto& point : points) {
        std::cout << "Point: (" << point.x << ", " << point.y << ", " << point.z << ")\n";
    }
}