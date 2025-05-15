#include "utils.hpp"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
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

struct Vec2 {
    float x, y;

    bool operator==(const Vec2& other) const
    {
        return std::fabs(x - other.x) < 1e-6f && std::fabs(y - other.y) < 1e-6f;
    }
};

struct Vec2Hash {
    size_t operator()(const Vec2& v) const noexcept
    {
        // Quantize each component by EPSILON before hashing
        int ix = static_cast<int>(std::round(v.x / 1e-6f));
        int iy = static_cast<int>(std::round(v.y / 1e-6f));
        size_t hx = std::hash<int>()(ix);
        size_t hy = std::hash<int>()(iy);
        // Combine hashes
        return hx ^ (hy << 1);
    }
};

// Helper struct for deduplicating vertices with tolerance
struct Vec3 {
    float x, y, z;
    static constexpr float EPSILON = 1e-6f;

    bool operator==(const Vec3& other) const
    {
        return std::fabs(x - other.x) < EPSILON
            && std::fabs(y - other.y) < EPSILON
            && std::fabs(z - other.z) < EPSILON;
    }
};

struct Vec3Hash {
    size_t operator()(const Vec3& v) const noexcept
    {
        // Quantize each component by EPSILON before hashing
        int ix = static_cast<int>(std::round(v.x / Vec3::EPSILON));
        int iy = static_cast<int>(std::round(v.y / Vec3::EPSILON));
        int iz = static_cast<int>(std::round(v.z / Vec3::EPSILON));
        size_t hx = std::hash<int>()(ix);
        size_t hy = std::hash<int>()(iy);
        size_t hz = std::hash<int>()(iz);
        // Combine hashes
        return hx ^ (hy << 1) ^ (hz << 2);
    }
};

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;

    bool operator==(const Vertex& other) const
    {
        return position == other.position && normal == other.normal && texCoord == other.texCoord;
    }
};

struct VertexHash {
    size_t operator()(const Vertex& v) const noexcept
    {
        size_t h1 = Vec3Hash()(v.position);
        size_t h2 = Vec3Hash()(v.normal);
        size_t h3 = Vec2Hash()(v.texCoord);
        // Combine hashes
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

ModelInfo parseFile(const std::string& filename)
{
    ModelInfo modelInfo;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return modelInfo;
    }

    std::vector<float> rawPoints;
    std::vector<float> rawNormals;
    std::vector<float> rawTexCoords;
    std::vector<unsigned int> rawIndices;
    size_t numTriangles = 0;

    if (endsWith(filename, ".3d")) {
        size_t numPoints;
        file >> numPoints;
        rawPoints.resize(3 * numPoints);
        rawNormals.resize(3 * numPoints);
        rawTexCoords.resize(2 * numPoints);
        for (size_t i = 0; i < numPoints; ++i) {
            file >> rawPoints[3 * i] >> rawPoints[3 * i + 1] >> rawPoints[3 * i + 2];
            file >> rawNormals[3 * i] >> rawNormals[3 * i + 1] >> rawNormals[3 * i + 2];
            file >> rawTexCoords[2 * i] >> rawTexCoords[2 * i + 1];
        }

        file >> numTriangles;
        std::vector<int> associations(3 * numTriangles);
        for (size_t i = 0; i < numTriangles; ++i) {
            file >> associations[3 * i] >> associations[3 * i + 1] >> associations[3 * i + 2];
        }
        rawIndices.resize(associations.size());
        for (size_t i = 0; i < associations.size(); ++i) {
            rawIndices[i] = associations[i] - 1;
        }

        std::vector<float> uniquePoints;
        std::vector<unsigned int> newIndices;
        std::unordered_map<Vertex, int, VertexHash> indexMap;

        for (unsigned int idx : rawIndices) {
            Vec3 pos { rawPoints[3 * idx], rawPoints[3 * idx + 1], rawPoints[3 * idx + 2] };
            Vec3 norm { rawNormals[3 * idx], rawNormals[3 * idx + 1], rawNormals[3 * idx + 2] };
            Vec2 texCoord { rawTexCoords[2 * idx], rawTexCoords[2 * idx + 1] };
            Vertex v { pos, norm, texCoord };

            auto it = indexMap.find(v);
            if (it == indexMap.end()) {
                int newIdx = static_cast<int>(uniquePoints.size() / 3);
                uniquePoints.push_back(pos.x);
                uniquePoints.push_back(pos.y);
                uniquePoints.push_back(pos.z);
                modelInfo.normals.push_back(norm.x);
                modelInfo.normals.push_back(norm.y);
                modelInfo.normals.push_back(norm.z);
                modelInfo.texCoords.push_back(texCoord.x);
                modelInfo.texCoords.push_back(texCoord.y);
                indexMap[v] = newIdx;
                newIndices.push_back(newIdx);
            } else {
                newIndices.push_back(it->second);
            }
        }

        if (rawPoints.size() > uniquePoints.size()) {
            printf("Condensed %s from %zu raw floats to %zu unique floats\n", filename.c_str(), rawPoints.size(), uniquePoints.size());
        }

        modelInfo.points = std::move(uniquePoints);
        modelInfo.indices = std::move(newIndices);
        modelInfo.numTriangles = modelInfo.indices.size() / 3;
    }

    else if (endsWith(filename, ".obj")) {
        std::string line;
        std::vector<Vec3> tempPositions;
        std::vector<Vec3> tempNormals;
        std::vector<Vec2> tempTexCoords;

        struct ObjIndex {
            int posIdx = 0;
            int texIdx = 0;
            int normIdx = 0;
        };

        std::vector<ObjIndex> objIndices;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                tempPositions.push_back({ x, y, z });
            } else if (prefix == "vt") {
                float u, v;
                iss >> u >> v;
                tempTexCoords.push_back({ u, v });
            } else if (prefix == "vn") {
                float nx, ny, nz;
                iss >> nx >> ny >> nz;
                tempNormals.push_back({ nx, ny, nz });
            } else if (prefix == "f") {
                std::string vStr;
                for (int i = 0; i < 3; ++i) {
                    ObjIndex idx;
                    iss >> vStr;
                    size_t p1 = vStr.find('/');
                    size_t p2 = vStr.find('/', p1 + 1);

                    idx.posIdx = std::stoi(vStr.substr(0, p1)) - 1;
                    if (p1 != std::string::npos && p2 != std::string::npos && p2 > p1 + 1)
                        idx.texIdx = std::stoi(vStr.substr(p1 + 1, p2 - p1 - 1)) - 1;
                    if (p2 != std::string::npos)
                        idx.normIdx = std::stoi(vStr.substr(p2 + 1)) - 1;

                    objIndices.push_back(idx);
                }
            }
        }

        for (const ObjIndex& idx : objIndices) {
            Vec3 pos = tempPositions[idx.posIdx];
            Vec2 tex = (idx.texIdx >= 0 && idx.texIdx < (int)tempTexCoords.size()) ? tempTexCoords[idx.texIdx] : Vec2 { 0.0f, 0.0f };
            Vec3 norm = (idx.normIdx >= 0 && idx.normIdx < (int)tempNormals.size()) ? tempNormals[idx.normIdx] : Vec3 { 0.0f, 0.0f, 0.0f };

            modelInfo.points.push_back(pos.x);
            modelInfo.points.push_back(pos.y);
            modelInfo.points.push_back(pos.z);

            modelInfo.texCoords.push_back(tex.x);
            modelInfo.texCoords.push_back(tex.y);

            modelInfo.normals.push_back(norm.x);
            modelInfo.normals.push_back(norm.y);
            modelInfo.normals.push_back(norm.z);

            modelInfo.indices.push_back(static_cast<unsigned int>(modelInfo.indices.size()));
        }

        modelInfo.numTriangles = modelInfo.indices.size() / 3;
    }

    file.close();
    return modelInfo;
}
