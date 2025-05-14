#include "PointsGenerator.hpp"

#ifdef __APPLE__
#define CAST_POINT (Point)
#define CAST_ASSOCIATION (Association)
#else
#define CAST_POINT
#define CAST_ASSOCIATION
#endif

void PointsGenerator::addPoint(float x, float y, float z, float nx, float ny, float nz, float tx, float ty)
{
    points.push_back(CAST_POINT { x, y, z, nx, ny, nz, tx, ty });
}

void PointsGenerator::addAssociation(int p1, int p2, int p3)
{
    associations.push_back(CAST_ASSOCIATION { p1, p2, p3 });
}
