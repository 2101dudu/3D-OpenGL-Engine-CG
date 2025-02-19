#include "FileWriter.hpp"
#include "box.hpp"
#include "PointsGenerator.hpp"
#include "plane.hpp"
#include "sphere.hpp"

int main() {
  PointsGenerator generator;

    //generator.showData();

    Box::createBox(2, 1);
    //Plane::createPlane(2, 3);

  //Plane::createPlane(2, 3);
  Sphere::createSphere(1, 10, 10);


  return 0;
}
