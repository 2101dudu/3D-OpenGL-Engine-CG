#ifndef PointsGenerator_HPP
#define PointsGenerator_HPP

#include <iostream>
#include <vector>

// Structure to represent a 3D point
struct Point {
    float x, y, z;
};

// Structure to represent an association between 3 points (triangles)
struct Association {
    int p1, p2, p3;
};

// Class to generate points and associations
class PointsGenerator {
public:
    std::vector<Point> points;
    std::vector<Association> associations;

    void addPoint(float x, float y, float z);
    void addAssociation(int p1, int p2, int p3);
};

#endif
