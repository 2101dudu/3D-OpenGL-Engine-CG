#include "FileWriter.hpp"
#include "PointsGenerator.hpp"
#include "plane.hpp"

int main() {
  PointsGenerator generator;

  // generator.addPoint(0.0, 1.0, 0.0);
  // generator.addPoint(-1.0, -1.0, 1.0);
  // generator.addPoint(1.0, -1.0, 1.0);
  // generator.addPoint(-1.0, -1.0, -1.0);
  // generator.addPoint(1.0, -1.0, -1.0);

  // generator.addAssociation(1, 2, 3);
  // generator.addAssociation(2, 4, 5);

  Plane::createPlane(2, 3);

  return 0;
}
