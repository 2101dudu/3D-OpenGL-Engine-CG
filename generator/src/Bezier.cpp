// bezier.cpp
#include "Bezier.hpp"
#include "FileWriter.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

struct Vec3 {
    float x, y, z;
    Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }
    Vec3 operator+(const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
};

// Bernstein polynomial of degree 3
static float bernstein(int i, float t)
{
    switch (i) {
    case 0:
        return (1 - t) * (1 - t) * (1 - t);
    case 1:
        return 3 * t * (1 - t) * (1 - t);
    case 2:
        return 3 * t * t * (1 - t);
    case 3:
        return t * t * t;
    }
    return 0.0f;
}

// Evaluate a 3x3-degree Bézier patch at (u,v)
static Vec3 evaluateBezierPatch(const std::vector<Vec3>& cp, float u, float v)
{
    Vec3 point { 0, 0, 0 };
    for (int i = 0; i < 4; ++i) {
        float bu = bernstein(i, u);
        for (int j = 0; j < 4; ++j) {
            float bv = bernstein(j, v);
            point = point + cp[i * 4 + j] * (bu * bv);
        }
    }
    return point;
}
//
// derivative of Bernstein basis of degree 3
static float bernsteinD(int i, float t)
{
    switch (i) {
    case 0:
        return -3 * (1 - t) * (1 - t);
    case 1:
        return 3 * (1 - t) * (1 - t) - 6 * t * (1 - t);
    case 2:
        return 6 * t * (1 - t) - 3 * t * t;
    case 3:
        return 3 * t * t;
    }
    return 0.0f;
}

static Vec3 evalDu(const std::vector<Vec3>& cp, float u, float v)
{
    Vec3 du { 0, 0, 0 };
    for (int i = 0; i < 4; ++i) {
        float bu = bernsteinD(i, u);
        for (int j = 0; j < 4; ++j) {
            float bv = bernstein(j, v);
            du = du + cp[i * 4 + j] * (bu * bv);
        }
    }
    return du;
}

static Vec3 evalDv(const std::vector<Vec3>& cp, float u, float v)
{
    Vec3 dv { 0, 0, 0 };
    for (int i = 0; i < 4; ++i) {
        float bu = bernstein(i, u);
        for (int j = 0; j < 4; ++j) {
            float bv = bernsteinD(j, v);
            dv = dv + cp[i * 4 + j] * (bu * bv);
        }
    }
    return dv;
}

static Vec3 cross(const Vec3& a, const Vec3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static Vec3 normalize(const Vec3& v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len == 0)
        return { 0, 0, 0 };
    return { v.x / len, v.y / len, v.z / len };
}

void Bezier::createBezierModel(const std::string& patchFile,
    int tessellationLevel,
    const std::string& outputFile)
{
    std::ifstream file(patchFile);
    if (!file) {
        std::cerr << "Error: Cannot open patch file: " << patchFile << std::endl;
        return;
    }

    int numPatches;
    file >> numPatches;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<std::vector<int>> patches;
    patches.reserve(numPatches);
    for (int i = 0; i < numPatches; ++i) {
        std::string line;
        std::getline(file, line);
        std::stringstream ss(line);
        std::vector<int> idxs;
        std::string tok;
        while (std::getline(ss, tok, ',')) {
            idxs.push_back(std::stoi(tok)); // assume 0-based indices in .patch file
        }
        if (idxs.size() == 16)
            patches.push_back(idxs);
        else
            std::cerr << "Warning: Patch #" << i << " has " << idxs.size() << " indices\n";
    }

    int numControlPoints;
    file >> numControlPoints;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<Vec3> controlPoints;
    controlPoints.reserve(numControlPoints);
    for (int i = 0; i < numControlPoints; ++i) {
        std::string line;
        if (!std::getline(file, line))
            break;
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream ciss(line);
        Vec3 cp { 0, 0, 0 };
        ciss >> cp.x >> cp.y >> cp.z;
        controlPoints.push_back(cp);
    }

    PointsGenerator generator;

    for (size_t p = 0; p < patches.size(); ++p) {
        const auto& patchIdx = patches[p];

        // Build 4×4 control grid
        std::vector<Vec3> patchCP(16);
        for (int u = 0; u < 4; ++u) {
            for (int v = 0; v < 4; ++v) {
                int fileIndex = patchIdx[v * 4 + u];
                patchCP[u * 4 + v] = controlPoints[fileIndex];
            }
        }

        int rows = tessellationLevel + 1;
        std::vector<std::vector<int>> idx(rows, std::vector<int>(rows));

        // Generate shared grid of evalued points
        for (int i = 0; i < rows; ++i) {
            float fu = float(i) / tessellationLevel;
            for (int j = 0; j < rows; ++j) {
                float fv = float(j) / tessellationLevel;
                Vec3 pt = evaluateBezierPatch(patchCP, fu, fv);
                Vec3 du = evalDu(patchCP, fu, fv);
                Vec3 dv = evalDv(patchCP, fu, fv);
                Vec3 n = normalize(cross(du, dv));
                generator.addPoint(pt.x, pt.y, pt.z, n.x, n.y, n.z, fu, fv);

                int pointIndex = generator.getPoints().size();
                idx[i][j] = pointIndex;
            }
        }

        // Emit two triangles per quad with consistent CCW winding
        for (int i = 0; i < tessellationLevel; ++i) {
            for (int j = 0; j < tessellationLevel; ++j) {
                int a = idx[i][j];
                int b = idx[i + 1][j];
                int c = idx[i + 1][j + 1];
                int d = idx[i][j + 1];
                generator.addAssociation(a, b, c);
                generator.addAssociation(a, c, d);
            }
        }
    }

    FileWriter::writeToFile(outputFile, generator);
}
