// PointsGenerator.hpp
#pragma once
#include <vector>

#ifdef __APPLE__
#define CAST_POINT (Point)
#define CAST_ASSOCIATION (Association)
#else
#define CAST_POINT
#define CAST_ASSOCIATION
#endif

struct Point {
    float x, y, z;
    float nx, ny, nz;
};

struct Association {
    int p1, p2, p3;
};

class PointsGenerator {
public:
    void addPoint(float x, float y, float z, float nx, float ny, float nz);
    void addAssociation(int p1, int p2, int p3);

    const std::vector<Point>& getPoints() const { return points; }
    const std::vector<Association>& getAssociations() const { return associations; }

private:
    std::vector<Point> points;
    std::vector<Association> associations;
};
