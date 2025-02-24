//
// Created by Eduardo Faria on 21/02/2025.
//

#ifndef TORUS_H
#define TORUS_H

#include "FileWriter.hpp"
#include "PointsGenerator.hpp"

class Torus {
public:
    static void createTorus(float radius, float minorRadius, int slices, int stacks, const std::string& filename);
};

#endif // TORUS_H
