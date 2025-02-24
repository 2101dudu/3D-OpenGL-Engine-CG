#include "utils.hpp"
#include <fstream>
#include <iostream>

std::vector<float> parseFile(const std::string& filename)
{
    std::vector<float> points;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return points;
    }

    size_t numPoints, numAssociations;
    file >> numPoints;
    std::vector<float> allPoints(3 * numPoints);
    for (size_t i = 0; i < numPoints; ++i) {
        file >> allPoints[3 * i] >> allPoints[3 * i + 1] >> allPoints[3 * i + 2];
    }

    file >> numAssociations;
    std::vector<int> associations(3 * numAssociations);
    for (size_t i = 0; i < numAssociations; ++i) {
        file >> associations[3 * i] >> associations[3 * i + 1] >> associations[3 * i + 2];
    }

    for (size_t i = 0; i < numAssociations; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            size_t index = (associations[3 * i + j] - 1) * 3;
            points.push_back(allPoints[index]);
            points.push_back(allPoints[index + 1]);
            points.push_back(allPoints[index + 2]);
        }
    }

    file.close();
    return points;
}
