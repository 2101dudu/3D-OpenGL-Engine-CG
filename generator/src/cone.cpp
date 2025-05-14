#include "cone.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

void Cone::createCone(float radius, float height, int slices, int stacks, const std::string& filename)
{
    PointsGenerator generator;
    float stackHeight = height / stacks;
    float angleStep = 2 * M_PI / slices;

    int offset = 0;
    for (int i = -1; i <= stacks; ++i) {
        int index = std::max(i, 0);
        float currentHeight = index * stackHeight;
        float currentRadius = radius * (1 - (float)index / stacks);
        for (int j = 0; j < slices; ++j) {
            float angle = j * angleStep;
            float x = currentRadius * cos(angle);
            float y = currentHeight;
            float z = currentRadius * sin(angle);

            if (i == -1) {
                generator.addPoint(x, 0, z, 0, -1, 0, 0, 0);
                offset++;
                continue;
            }

            float slantLength = sqrt(radius * radius + height * height);
            float n1 = cos(angle) * height / slantLength;
            float n2 = radius / slantLength;
            float n3 = sin(angle) * height / slantLength;

            generator.addPoint(x, y, z, n1, n2, n3, 0, 0);

            int currPointIndex = i * slices + (j + 1) + offset;
            int neighbourPointIndex = (currPointIndex + 1) % (slices * (i + 1) + 1 + offset);
            neighbourPointIndex += neighbourPointIndex == 0 ? 1 + slices * i + offset : 0;

            int p1 = currPointIndex;
            int p2 = neighbourPointIndex;
            int p3 = neighbourPointIndex + slices;
            int p4 = currPointIndex + slices;

            if (i < stacks - 1) {
                generator.addAssociation(p1, p3, p2);
                generator.addAssociation(p1, p4, p3);
            } else if (i < stacks) {
                generator.addAssociation(p1, p4, p2);
            }
        }
    }

    generator.addPoint(0, 0, 0, 0, -1, 0, 0, 0);
    int baseCenterIndex = generator.getPoints().size();

    // if we have 9 slices, i.e., 9 points, we need to
    // have N associations
    for (int j = 0; j < slices; j++) {
        int p1 = j + 1;
        int p2 = (j + 2) % (slices + 1); // look for the neighbor point, but ensure it doesn't overflow
        p2 += p2 == 0 ? 1 : 0; // if a points overflows, the result will be the index 0, which, for now, cannot happen
        generator.addAssociation(p1, p2, baseCenterIndex);
    }

    FileWriter::writeToFile(filename, generator);
}
