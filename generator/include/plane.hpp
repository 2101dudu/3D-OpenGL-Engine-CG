//
// Created by Eduardo Faria on 15/02/2025.
//

#ifndef PLANE_H
#define PLANE_H

#include "PointsGenerator.hpp"

class Plane {
public:
    // a square in the XZ plane, centred in the origin, subdivided in both X and Z directions
    static void createPlane(int dimension, int divPerEdge);
};

#endif //PLANE_H
