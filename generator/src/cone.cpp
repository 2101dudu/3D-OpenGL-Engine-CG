#include "cone.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

void Cone::createCone(float radius, float height, int slices, int stacks) {
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

    int tipIndex = generator.points.size();
    generator.addPoint(0, height, 0);
    for (int j = 0; j < slices; ++j) {
        int p1 = (stacks - 1) * (slices + 1) + j;
        int p2 = p1 + 1;
        generator.addAssociation(p1, p2, tipIndex);
    }

    int baseCenterIndex = generator.points.size();
    generator.addPoint(0, 0, 0);
    for (int j = 0; j < slices; ++j) {
        int p1 = j;
        int p2 = (j + 1) % slices;
        generator.addAssociation(p1, baseCenterIndex, p2);
    }

    FileWriter::writeToFile("pontos.txt", generator);
}
