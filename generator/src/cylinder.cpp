#include "cylinder.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

void Cylinder::createCylinder(int radius, int height, int slices, int stacks)
{
    PointsGenerator pointGen;

    float angleStep = 2.0f * M_PI / slices;
    float stackHeight = static_cast<float>(height) / stacks;

    int indexCounter = 1;

    for (int i = 0; i <= stacks; ++i) {
        float currentHeight = i * stackHeight;
        for (int j = 0; j < slices; ++j) {
            float angle = j * angleStep;
            float x = radius * cos(angle);
            float y = currentHeight;
            float z = radius * sin(angle);
            pointGen.addPoint(x, y, z);
            indexCounter++;
        }
    }

    pointGen.addPoint(0, 0, 0);
    int bottomCenterIndex = indexCounter++;

    pointGen.addPoint(0, height, 0);
    int topCenterIndex = indexCounter++;

    for (int j = 0; j < slices; ++j) {
        int next = (j + 1) % slices;
        pointGen.addAssociation(bottomCenterIndex, j, next);
    }

    int topStart = stacks * slices;
    for (int j = 0; j < slices; ++j) {
        int next = (j + 1) % slices;
        pointGen.addAssociation(topCenterIndex, topStart + next, topStart + j);
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int next = (j + 1) % slices;
            int current = i * slices + j;
            int nextStack = (i + 1) * slices + j;
            int currentNext = i * slices + next;
            int nextStackNext = (i + 1) * slices + next;

            pointGen.addAssociation(current, nextStack, currentNext);
            pointGen.addAssociation(currentNext, nextStack, nextStackNext);
        }
    }

    pointGen.showData();
    FileWriter::writeToFile("pontos.txt", pointGen);
}
