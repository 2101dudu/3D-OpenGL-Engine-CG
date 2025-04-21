#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

struct Association {
    int p1, p2, p3;
};

struct ModelInfo {
    std::vector<float> points;
    std::vector<unsigned int> indices;    // ← NOVO: índices
    int64_t numTriangles;                 // em triângulos
};

ModelInfo parseFile(const std::string& filename);

#endif
