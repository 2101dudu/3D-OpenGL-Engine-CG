#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() >= suffix.length()) {
        return (0 == str.compare(str.length() - suffix.length(), suffix.length(), suffix));
    }
    else {
        return false;
    }
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.rfind(prefix, 0) == 0;
}

std::vector<float> parseFile(const std::string& filename)
{
    std::vector<float> points;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return points;
    }

    if (endsWith(filename, ".3d")) {
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
    }
    else if (endsWith(filename, ".obj")) {
        std::vector<float> allPoints;
        std::vector<int> associations;
        std::string line;

        while (std::getline(file, line)) {
            if (startsWith(line, "v ")) { // Vertex
                float x, y, z;
                sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
                points.push_back(x);
                points.push_back(y);
                points.push_back(z);
            }
            else if (startsWith(line, "f ")) { // Face
                int a, b, c;
                sscanf(line.c_str(), "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &a, &b, &c); // Get first vertex index of each face element
                associations.push_back(a - 1); // Convert to 0-based index
                associations.push_back(b - 1);
                associations.push_back(c - 1);
            }
        }
    }
    else {
        std::cerr << "Error: Unsupported file format " << filename << std::endl;
    }

    file.close();
    return points;
}
