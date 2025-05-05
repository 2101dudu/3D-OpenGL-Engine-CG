#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

struct Association {
    int p1, p2, p3;
};

struct ModelInfo {
    std::vector<float> points;
    std::vector<float> normals;
    std::vector<unsigned int> indices;
    int64_t numTriangles;
};

ModelInfo parseFile(const std::string& filename);

#endif
