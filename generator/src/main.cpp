#include "FileWriter.hpp"
#include "PointsGenerator.hpp"
#include "box.hpp"
#include "plane.hpp"
#include "sphere.hpp"
#include "torus.hpp"

int main()
{
    PointsGenerator generator;

    // generator.showData();

    // Box::createBox(2, 1);
    //  Plane::createPlane(2, 3);

    // Plane::createPlane(2, 3);
    // Sphere::createSphere(1, 10, 10);
    Torus::createTorus(1.5, 0.6, 40, 15);

    return 0;
}
