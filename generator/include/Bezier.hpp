#ifndef BEZIER_HPP
#define BEZIER_HPP

#include <string>
#include "PointsGenerator.hpp"

class Bezier {
public:
    static void createBezierModel(const std::string& patchFile, int tessellationLevel, const std::string& outputFile);
};

#endif
