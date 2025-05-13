#include "cylinder.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

void Cylinder::createCylinder(int radius, int height, int slices, int stacks, const std::string& filename)
{
    PointsGenerator pointGen;

    float angleStep = 2.0f * M_PI / slices;
    float stackHeight = static_cast<float>(height) / stacks;

    int offset = 0;
    for (int i = -1; i <= stacks + 1; ++i) {
        float currentHeight = i * stackHeight;
        for (int j = 0; j < slices; ++j) {
            float angle = j * angleStep;
            float x = radius * cos(angle);
            float y = currentHeight;
            float z = radius * sin(angle);

            if (i == -1) {
                pointGen.addPoint(x, 0, z, 0, -1, 0, 0, 0);
                offset++;
                continue;
            }

            if (i == stacks + 1) {
                pointGen.addPoint(x, height, z, 0, 1, 0, 0, 0);
            } else {
                pointGen.addPoint(x, y, z, x / radius, 0, z / radius, 0, 0);
            }

            int index = i;
            int currPointIndex = index * slices + (j + 1) + offset;
            int neighbourPointIndex = (currPointIndex + 1) % (slices * (index + 1) + 1 + offset);
            neighbourPointIndex += neighbourPointIndex == 0 ? 1 + slices * index + offset : 0;

            int p1 = currPointIndex;
            int p2 = neighbourPointIndex;
            int p3 = neighbourPointIndex + slices;
            int p4 = currPointIndex + slices;

            if (i < stacks) {
                pointGen.addAssociation(p1, p3, p2);
                pointGen.addAssociation(p1, p4, p3);
            } else if (i == stacks + 1) {
                int topCenterIndex = slices * (stacks + 1 + 1) + 1 + offset;
                pointGen.addAssociation(p1, topCenterIndex, p2);
            }
        }
    }

    // now we add the top-monst centered point
    pointGen.addPoint(0, height, 0, 0, 1, 0, 0, 0);

    pointGen.addPoint(0, 0, 0, 0, -1, 0, 0, 0);
    int bottomCenterIndex = pointGen.getPoints().size();

    for (int j = 0; j < slices; ++j) {
        int p1 = j + 1;
        int p2 = (j + 2) % (slices + 1); // look for the neighbor point, but ensure it doesn't overflow
        p2 += p2 == 0 ? 1 : 0; // if a points overflows, the result will be the index 0, which, for now, cannot happen
        pointGen.addAssociation(p1, p2, bottomCenterIndex);
    }

    FileWriter::writeToFile(filename, pointGen);
}
