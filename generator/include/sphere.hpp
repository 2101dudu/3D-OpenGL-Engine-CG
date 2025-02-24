//
// Created by Eduardo Faria on 19/02/2025.
//

#ifndef SPHERE_H
#define SPHERE_H

#include "FileWriter.hpp"
#include "PointsGenerator.hpp"

class Sphere {
public:
    //requires dimension, and the number of divisions per edge, centred in the origin

    static void createSphere(int radius, int slices, int stacks, const std::string& filename);
};

#endif //SPHERE_H
