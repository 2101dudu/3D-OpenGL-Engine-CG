#ifndef BOX_H
#define BOX_H

#include "FileWriter.hpp"
#include "PointsGenerator.hpp"

class Box {
public:
    // requires dimension, and the number of divisions per edge, centred in the origin
    static void createBox(int dimension, int divPerEdge, const std::string& filename);
};

#endif // BOX_H
