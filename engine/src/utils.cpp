#include "utils.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

bool endsWith(const std::string& str, const std::string& suffix)
{
    if (str.length() >= suffix.length()) {
        return (0 == str.compare(str.length() - suffix.length(), suffix.length(), suffix));
    } else {
        return false;
    }
}

bool startsWith(const std::string& str, const std::string& prefix)
{
    return str.rfind(prefix, 0) == 0;
}

ModelInfo parseFile(const std::string& filename)
{
    ModelInfo modelInfo;

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return modelInfo;
    }

    if (endsWith(filename, ".3d")) {
        size_t numPoints;
        file >> numPoints;
        std::vector<float> allPoints(3 * numPoints);
        for (size_t i = 0; i < numPoints; ++i) {
            file >> allPoints[3 * i] >> allPoints[3 * i + 1] >> allPoints[3 * i + 2];
        }

        file >> modelInfo.numTriangles;
        std::vector<int> associations(3 * modelInfo.numTriangles);
        for (size_t i = 0; i < modelInfo.numTriangles; ++i) {
            file >> associations[3 * i] >> associations[3 * i + 1] >> associations[3 * i + 2];
        }

        for (size_t i = 0; i < modelInfo.numTriangles; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                size_t index = (associations[3 * i + j] - 1) * 3;
                modelInfo.points.push_back(allPoints[index]);
                modelInfo.points.push_back(allPoints[index + 1]);
                modelInfo.points.push_back(allPoints[index + 2]);
            }
        }
    } else if (endsWith(filename, ".obj")) {
        std::vector<float> allPoints;
        std::vector<int> associations;
        std::string line;

        while (std::getline(file, line)) {
            if (startsWith(line, "v ")) { // Vertex
                float x, y, z;
                sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
                modelInfo.points.push_back(x);
                modelInfo.points.push_back(y);
                modelInfo.points.push_back(z);
            } else if (startsWith(line, "f ")) { // Face
                int a, b, c;
                sscanf(line.c_str(), "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &a, &b, &c); // Get first vertex index of each face element
                associations.push_back(a - 1); // Convert to 0-based index
                associations.push_back(b - 1);
                associations.push_back(c - 1);
            }
        }

        modelInfo.numTriangles = associations.size();
    } else {
        std::cerr << "Error: Unsupported file format " << filename << std::endl;
    }

    file.close();
    return modelInfo;
}
