//
// Created by Eduardo Faria on 15/02/2025.
//

#ifndef BOX_H
#define BOX_H

#include "PointsGenerator.hpp"

class Box {
public:
    // requires dimension, and the number of divisions per edge, centred in the origin
    static void createBox(int dimension, int divPerEdge);
};

#endif //BOX_H
