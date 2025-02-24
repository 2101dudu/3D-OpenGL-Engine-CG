#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

struct Association {
    int p1, p2, p3;
};

std::vector<float> parseFile(const std::string& filename);

#endif
