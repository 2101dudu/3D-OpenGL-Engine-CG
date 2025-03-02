//
// Created by Eduardo Faria on 21/02/2025.
//

#include "torus.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

void Torus::createTorus(float radius, float minorRadius, int slices, int stacks, const std::string& filename)
{
    PointsGenerator pointGen;

    // angle per slice
    float alpha = 2 * M_PI / slices;

    // angle per stack
    float beta = 2 * M_PI / stacks;

    for (int i = 0; i < slices; i++) {
        float currAlpha = alpha * i;

        for (int j = 0; j < stacks; j++) {
            float currBeta = beta * j;

            float x = (radius + minorRadius * cosf(currBeta)) * cosf(currAlpha);
            float y = minorRadius * sinf(currBeta);
            float z = (radius + minorRadius * cosf(currBeta)) * sinf(currAlpha);

            pointGen.addPoint(x, y, z);

            int currPointIndex = i * stacks + j + 1;
            int nextCurrPointIndex = i * stacks + ((j + 1) % stacks) + 1;
            int neighbourIndex = ((i + 1) % slices) * stacks + j + 1;
            int nextNeighbourIndex = ((i + 1) % slices) * stacks + ((j + 1) % stacks) + 1;

            pointGen.addAssociation(currPointIndex, nextCurrPointIndex, neighbourIndex);
            pointGen.addAssociation(neighbourIndex, nextCurrPointIndex, nextNeighbourIndex);
        }
    }

    FileWriter::writeToFile(filename, pointGen);
}
