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

    for (int i = 0; i <= slices; i++) {
        float currAlpha = alpha * i;

        for (int j = 0; j <= stacks; j++) {
            float currBeta = beta * j;

            float x = (radius + minorRadius * cosf(currBeta)) * cosf(currAlpha);
            float y = minorRadius * sinf(currBeta);
            float z = (radius + minorRadius * cosf(currBeta)) * sinf(currAlpha);

            pointGen.addPoint(x, y, z);

            int currPointIndex = (stacks + 1) * i + j + 1;

            int neighbourIndex = (currPointIndex + stacks + 1) % ((slices + 1) * (stacks + 1));

            if (j < stacks) {
                pointGen.addAssociation(currPointIndex, currPointIndex + 1, neighbourIndex);
                pointGen.addAssociation(neighbourIndex, currPointIndex + 1, neighbourIndex + 1);
            }
        }
    }

    FileWriter::writeToFile(filename, pointGen);
}
