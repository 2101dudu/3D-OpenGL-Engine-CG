#ifndef CYLINDER_H
#define CYLINDER_H

#include "FileWriter.hpp"
#include "PointsGenerator.hpp"

class Cylinder {
public:
    //requires dimension, and the number of divisions per edge, centred in the origin

    static void createCylinder(int radius, int height, int slices, int stacks);
};

#endif // CYLINDER_H
