#include "PointsGenerator.hpp"

#ifdef __APPLE__
#define CAST_POINT (Point)
#define CAST_ASSOCIATION (Association)
#else
#define CAST_POINT
#define CAST_ASSOCIATION
#endif

void PointsGenerator::addPoint(float x, float y, float z)
{
    points.push_back(CAST_POINT { x, y, z });
}

void PointsGenerator::addAssociation(int p1, int p2, int p3)
{
    associations.push_back(CAST_ASSOCIATION { p1, p2, p3 });
}

void PointsGenerator::showData() const
{
    std::cout << points.size() << "\n";
    for (const auto& p : points) {
        std::cout << p.x << " " << p.y << " " << p.z << "\n";
    }
    std::cout << associations.size() << "\n";
    for (const auto& a : associations) {
        std::cout << a.p1 << " " << a.p2 << " " << a.p3 << "\n";
    }
}
