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
        size_t numPoints; file >> numPoints;
        std::vector<float> allPoints(3 * numPoints);
        for (size_t i = 0; i < numPoints; ++i)
            file >> allPoints[3*i] >> allPoints[3*i+1] >> allPoints[3*i+2];

        file >> modelInfo.numTriangles;
        // lê associações (1-based)
        std::vector<int> assoc(3 * modelInfo.numTriangles);
        for (size_t i = 0; i < modelInfo.numTriangles; ++i)
            file >> assoc[3*i] >> assoc[3*i+1] >> assoc[3*i+2];

        // passa vértices únicos
        modelInfo.points = std::move(allPoints);
        // constrói índices 0-based
        modelInfo.indices.resize(assoc.size());
        for (size_t i = 0; i < assoc.size(); ++i)
            modelInfo.indices[i] = assoc[i] - 1;

    } else if (endsWith(filename, ".obj")) {
        std::vector<float> allPoints;
        std::vector<int> assoc;
        std::string line;
        while (std::getline(file, line)) {
            if (startsWith(line, "v ")) {
                float x,y,z; sscanf(line.c_str(),"v %f %f %f",&x,&y,&z);
                allPoints.push_back(x);
                allPoints.push_back(y);
                allPoints.push_back(z);
            } else if (startsWith(line, "f ")) {
                int a,b,c;
                sscanf(line.c_str(),"f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d",&a,&b,&c);
                assoc.push_back(a-1);
                assoc.push_back(b-1);
                assoc.push_back(c-1);
            }
        }
        modelInfo.points = std::move(allPoints);
        modelInfo.indices.resize(assoc.size());
        for (size_t i = 0; i < assoc.size(); ++i)
            modelInfo.indices[i] = assoc[i];
        modelInfo.numTriangles = assoc.size()/3;
    }

    file.close();
    return modelInfo;
}
