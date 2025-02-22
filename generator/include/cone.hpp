#ifndef CONE_H
#define CONE_H

#include "FileWriter.hpp"
#include "PointsGenerator.hpp"

class Cone {
public:
    // requires radius, height, slices, and stacks
    static void createCone(float radius, float height, int slices, int stacks);
};

#endif // CONE_H
