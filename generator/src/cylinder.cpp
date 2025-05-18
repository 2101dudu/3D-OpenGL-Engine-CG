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
        for (int j = 0; j <= slices; ++j) {
            float angle = j * angleStep;
            float x = radius * cos(angle);
            float y = currentHeight;
            float z = radius * sin(angle);

            float u = static_cast<float>(j) / (slices + 1);
            float v = static_cast<float>(i) / (stacks + 1);

            if (i == -1) {
                // base inferior
                float base_u = 0.5f + (x / (2.0f * radius));
                float base_v = 0.5f + (z / (2.0f * radius));
                pointGen.addPoint(x, 0, z, 0, -1, 0, base_u, base_v);
                offset++;
                continue;
            }

            if (i == stacks + 1) {
                // base superior
                float top_u = 0.5f + (x / (2.0f * radius));
                float top_v = 0.5f + (z / (2.0f * radius));
                pointGen.addPoint(x, height, z, 0, 1, 0, top_u, top_v);
            } else {
                // lateral
                pointGen.addPoint(x, y, z, x / radius, 0, z / radius, u, v);
            }

            int index = i;
            int currPointIndex = index * (slices + 1) + (j + 1) + offset;
            int neighbourPointIndex = (currPointIndex + 1) % ((slices + 1) * (index + 1) + 1 + offset);
            neighbourPointIndex += neighbourPointIndex == 0 ? 1 + (slices + 1) * index + offset : 0;

            int p1 = currPointIndex;
            int p2 = neighbourPointIndex;
            int p3 = neighbourPointIndex + slices + 1;
            int p4 = currPointIndex + slices + 1;

            if (i < stacks) {
                pointGen.addAssociation(p1, p3, p2);
                pointGen.addAssociation(p1, p4, p3);
            } else if (i == stacks + 1) {
                int topCenterIndex = (slices + 1) * (stacks + 1 + 1) + 1 + offset;
                pointGen.addAssociation(p1, topCenterIndex, p2);
            }
        }
    }

    // centro do topo
    pointGen.addPoint(0, height, 0, 0, 1, 0, 0.5f, 0.5f);

    // centro da base inferior
    pointGen.addPoint(0, 0, 0, 0, -1, 0, 0.5f, 0.5f);
    int bottomCenterIndex = pointGen.getPoints().size();

    for (int j = 0; j < slices; ++j) {
        int p1 = j + 1;
        int p2 = (j + 2) % (slices + 1);
        p2 += p2 == 0 ? 1 : 0;
        pointGen.addAssociation(p1, p2, bottomCenterIndex);
    }

    FileWriter::writeToFile(filename, pointGen);
}
