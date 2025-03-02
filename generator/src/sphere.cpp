//
// Created by Eduardo Faria on 19/02/2025.
//

#include "sphere.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

void Sphere::createSphere(int radius, int slices, int stacks, const std::string& filename)
{
    PointsGenerator pointGen;

    // angle per slice
    float alpha = 2 * M_PI / slices;

    // angle per stack
    float beta = M_PI / stacks;

    for (int i = 0; i < slices; i++) {
        float currAlpha = alpha * i;

        for (int j = 0; j <= stacks; j++) {
            float currBeta = static_cast<float>(-M_PI / 2) + beta * j;

            float x = cosf(currAlpha) * radius * cos(currBeta);
            float y = sinf(currBeta) * radius;
            float z = sinf(currAlpha) * radius * cos(currBeta);

            pointGen.addPoint(x, y, z);

            int currPointIndex = (stacks + 1) * i + j + 1;

            int neighbourIndex = (currPointIndex + stacks + 1) % ((slices) * (stacks + 1));

            // association with polar points
            if (j == 1) {
                pointGen.addAssociation(currPointIndex - 1, currPointIndex, neighbourIndex);
            }
            if (j == stacks - 1) {
                pointGen.addAssociation(currPointIndex, currPointIndex + 1, neighbourIndex);
            }

            if (j > 0 && j < stacks - 1) {
                pointGen.addAssociation(currPointIndex, currPointIndex + 1, neighbourIndex);
                pointGen.addAssociation(neighbourIndex, currPointIndex + 1, neighbourIndex + 1);
            }
        }
    }

    FileWriter::writeToFile(filename, pointGen);
}
