#include "cone.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

void Cone::createCone(float radius, float height, int slices, int stacks)
{
    PointsGenerator generator;
    float stackHeight = height / stacks;
    float angleStep = 2 * M_PI / slices;

    for (int i = 0; i <= stacks; ++i) {
        float currentHeight = i * stackHeight;
        float currentRadius = radius * (1 - (float)i / stacks);
        for (int j = 0; j <= slices; ++j) {
            float angle = j * angleStep;
            float x = currentRadius * cos(angle);
            float y = currentHeight;
            float z = currentRadius * sin(angle);
            generator.addPoint(x, y, z);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j <= slices; ++j) {
            int p1 = i * (slices + 1) + j;
            int p2 = p1 + 1;
            int p3 = p1 + (slices + 1);
            int p4 = p3 + 1;

            generator.addAssociation(p1, p3, p2);
            generator.addAssociation(p2, p3, p4);
        }
    }

    generator.addPoint(0, 0, 0);
    int baseCenterIndex = generator.points.size();

    // if we have 9 slices, i.e., 9 points, we need to
    // have N + 1 associations
    for (int j = 0; j <= slices; j++) {
        int p1 = j + 1; // an index, for now, is never 0
        int p2 = (j + 2) % (slices + 1); // look for the neighbor point, but ensure it doesn't overflow
        p2 += p2 == 0 ? 1 : 0; // if a points overflows, the result will be the index 0, which, for now, cannot happen
        generator.addAssociation(p1, p2, baseCenterIndex);
    }

    FileWriter::writeToFile("pontos.txt", generator);
}
