#include "cylinder.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

void Cylinder::createCylinder(int radius, int height, int slices, int stacks, const std::string& filename)
{
    PointsGenerator pointGen;

    float angleStep = 2.0f * M_PI / slices;
    float stackHeight = static_cast<float>(height) / stacks;

    for (int i = 0; i <= stacks; ++i) {
        float currentHeight = i * stackHeight;
        for (int j = 0; j < slices; ++j) {
            float angle = j * angleStep;
            float x = radius * cos(angle);
            float y = currentHeight;
            float z = radius * sin(angle);
            pointGen.addPoint(x, y, z, 0, 0, 0);

            int currPointIndex = i * slices + (j + 1);
            int neighbourPointIndex = (currPointIndex + 1) % (slices * (i + 1) + 1);
            neighbourPointIndex += neighbourPointIndex == 0 ? 1 + slices * i : 0;

            int p1 = currPointIndex;
            int p2 = neighbourPointIndex;
            int p3 = neighbourPointIndex + slices;
            int p4 = currPointIndex + slices;

            if (i < stacks) {
                pointGen.addAssociation(p1, p3, p2);
                pointGen.addAssociation(p1, p4, p3);
            } else {
                // we can use the last iteration of the loops — that
                // would not be used to make "upward" associations — to
                // make the associations of the top part of the cylinder
                int topCenterIndex = slices * (stacks + 1) + 1;
                pointGen.addAssociation(p1, topCenterIndex, p2);
            }
        }
    }

    // now we add the top-monst centered point
    pointGen.addPoint(0, height, 0, 0, 0, 0);

    pointGen.addPoint(0, 0, 0, 0, 0, 0);
    int bottomCenterIndex = pointGen.getPoints().size();

    for (int j = 0; j < slices; ++j) {
        int p1 = j + 1; // an index, for now, is never 0
        int p2 = (j + 2) % (slices + 1); // look for the neighbor point, but ensure it doesn't overflow
        p2 += p2 == 0 ? 1 : 0; // if a points overflows, the result will be the index 0, which, for now, cannot happen
        pointGen.addAssociation(p1, p2, bottomCenterIndex);
    }

    FileWriter::writeToFile(filename, pointGen);
}
