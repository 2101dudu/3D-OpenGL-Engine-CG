#ifndef PLANE_H
#define PLANE_H

#include "FileWriter.hpp"
#include "PointsGenerator.hpp"

class Plane {
public:
    // a square in the XZ plane, centred in the origin, subdivided in both X and Z
    // directions
    static void createPlane(int dimension, int divPerEdge, const std::string& filename);
};

#endif // PLANE_H
