#ifndef PointsGenerator_HPP
#define PointsGenerator_HPP

#include <vector>

struct Point {
    float x, y, z;
    float nx, ny, nz;
    float tx, ty;
};

struct Association {
    int p1, p2, p3;
};

class PointsGenerator {
public:
    void addPoint(float x, float y, float z, float nx, float ny, float nz, float tx, float ty);
    void addAssociation(int p1, int p2, int p3);

    const std::vector<Point>& getPoints() const { return points; }
    const std::vector<Association>& getAssociations() const { return associations; }

private:
    std::vector<Point> points;
    std::vector<Association> associations;
};

#endif
