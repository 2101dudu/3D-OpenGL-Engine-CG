#include "Bezier.hpp"
#include "FileWriter.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

struct Vec3 {
    float x, y, z;

    Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }
    Vec3 operator+(const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
};

// Bernstein function of 3º degree
float bernstein(int i, float t) {
    switch (i) {
    case 0: return (1 - t) * (1 - t) * (1 - t);
    case 1: return 3 * t * (1 - t) * (1 - t);
    case 2: return 3 * t * t * (1 - t);
    case 3: return t * t * t;
    }
    return 0;
}

Vec3 evaluateBezierPatch(const std::vector<Vec3>& controlPoints, float u, float v) {
    Vec3 point = { 0, 0, 0 };
    for (int i = 0; i < 4; ++i) {
        float bu = bernstein(i, u);
        for (int j = 0; j < 4; ++j) {
            float bv = bernstein(j, v);
            const Vec3& cp = controlPoints[i * 4 + j];
            point = point + cp * (bu * bv);
        }
    }
    return point;
}

void Bezier::createBezierModel(const std::string& patchFile, int tessellationLevel, const std::string& outputFile) {
    std::ifstream file(patchFile);
    if (!file) {
        std::cerr << "Error: Cannot open patch file: " << patchFile << std::endl;
        return;
    }

    std::cout << "Patch file opened successfully!" << std::endl;

    int numPatches;
    file >> numPatches;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the rest of the line

    std::vector<std::vector<int>> patches;
    for (int i = 0; i < numPatches; ++i) {
        std::string line;
        std::getline(file, line);
        std::stringstream ss(line);
        std::vector<int> indices;
        std::string token;
        while (std::getline(ss, token, ',')) {
            indices.push_back(std::stoi(token));
        }
        if (indices.size() != 16) {
            std::cerr << "Patch #" << i << " does not have 16 indices. Found " << indices.size() << std::endl;
            continue;
        }
        patches.push_back(indices);
    }

    int numControlPoints;
    file >> numControlPoints;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the rest of the line

    std::vector<Vec3> controlPoints;
    std::string line;
    for (int i = 0; i < numControlPoints; ++i) {
        if (!std::getline(file, line)) {
            std::cerr << "Error: Not enough lines in the file to read all control points." << std::endl;
            break;
        }

        // Remove leading and trailing whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        if (line.empty()) {
            std::cerr << "Error: Empty line encountered while reading control point " << i << std::endl;
            controlPoints.push_back({ 0.0f, 0.0f, 0.0f });
            continue;
        }

        // Replace commas with spaces to allow correct parsing
        std::replace(line.begin(), line.end(), ',', ' ');

        std::istringstream ss(line);
        float x, y, z;
        if (ss >> x >> y >> z) {
            controlPoints.push_back({ x, y, z });
        }
        else {
            std::cerr << "Error reading control point " << i << ": Line content - " << line << std::endl;
            controlPoints.push_back({ 0.0f, 0.0f, 0.0f });
        }
    }

    if (controlPoints.size() != numControlPoints) {
        std::cerr << "Warning: Read " << controlPoints.size() << " control points, but expected " << numControlPoints << std::endl;
    }

    PointsGenerator generator;

    for (size_t patchIndex = 0; patchIndex < patches.size(); ++patchIndex) {
        const auto& patch = patches[patchIndex];
        std::vector<Vec3> patchPoints(16);

        for (int i = 0; i < 16; ++i) {
            if (patch[i] < 0 || patch[i] >= (int)controlPoints.size()) {
                std::cerr << "Patch " << patchIndex << " has invalid index: " << patch[i] << std::endl;
                goto skip_patch;
            }
            patchPoints[i] = controlPoints[patch[i]];
        }

        for (int i = 0; i < tessellationLevel; ++i) {
            float u0 = (float)i / tessellationLevel;
            float u1 = (float)(i + 1) / tessellationLevel;

            for (int j = 0; j < tessellationLevel; ++j) {
                float v0 = (float)j / tessellationLevel;
                float v1 = (float)(j + 1) / tessellationLevel;

                Vec3 p00 = evaluateBezierPatch(patchPoints, u0, v0);
                Vec3 p01 = evaluateBezierPatch(patchPoints, u0, v1);
                Vec3 p10 = evaluateBezierPatch(patchPoints, u1, v0);
                Vec3 p11 = evaluateBezierPatch(patchPoints, u1, v1);

                int idx00 = generator.points.size(); generator.addPoint(p00.x, p00.y, p00.z);
                int idx01 = generator.points.size(); generator.addPoint(p01.x, p01.y, p01.z);
                int idx10 = generator.points.size(); generator.addPoint(p10.x, p10.y, p10.z);
                int idx11 = generator.points.size(); generator.addPoint(p11.x, p11.y, p11.z);

                generator.addAssociation(idx00, idx10, idx11);
                generator.addAssociation(idx00, idx11, idx01);
            }
        }

    skip_patch:
        continue;
    }

    FileWriter::writeToFile(outputFile, generator);
}