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
        for (size_t i = 0; i < numPoints; ++i)
            file >> allPoints[3 * i] >> allPoints[3 * i + 1] >> allPoints[3 * i + 2];

        file >> modelInfo.numTriangles;
        // Reads associations (1-based)
        std::vector<int> associations(3 * modelInfo.numTriangles);
        for (size_t i = 0; i < modelInfo.numTriangles; ++i)
            file >> associations[3 * i] >> associations[3 * i + 1] >> associations[3 * i + 2];

        // Moves uniques vertex's
        modelInfo.points = std::move(allPoints);
        // Builds 0-based indexes
        modelInfo.indices.resize(associations.size());
        for (size_t i = 0; i < associations.size(); ++i)
            modelInfo.indices[i] = associations[i] - 1;

    } else if (endsWith(filename, ".obj")) {
        std::vector<float> allPoints;
        std::vector<int> associations;
        std::string line;
        while (std::getline(file, line)) {
            if (startsWith(line, "v ")) {
                float x,y,z;
                sscanf(line.c_str(),"v %f %f %f",&x,&y,&z);
                allPoints.push_back(x);
                allPoints.push_back(y);
                allPoints.push_back(z);
            } else if (startsWith(line, "f ")) {
                int a,b,c;
                sscanf(line.c_str(),"f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d",&a,&b,&c); // Get first vertex index of each face element
                associations.push_back(a-1); // Convert to 0-based index
                associations.push_back(b-1);
                associations.push_back(c-1);
            }
        }
        modelInfo.points = std::move(allPoints);
        modelInfo.indices.resize(associations.size());
        for (size_t i = 0; i < associations.size(); ++i)
            modelInfo.indices[i] = associations[i];
        modelInfo.numTriangles = associations.size()/3;
    }

    file.close();
    return modelInfo;
}
