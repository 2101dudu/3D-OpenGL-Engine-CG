#include "plane.hpp"

void Plane::createPlane(int dimension, int divPerEdge, const std::string& filename)
{
    PointsGenerator pointGen;

    // the interval specified by the dimension, i.e.,
    // dimension = 2 --> [-1,1]
    const float maxWidth = static_cast<float>(dimension) / 2;

    // the number of unique points per number of divisions,
    // i.e., 3 divisions means 4 points
    const int pointsPerEdge = divPerEdge + 1;

    for (int i = 0; i < pointsPerEdge; i++) {
        for (int j = 0; j < pointsPerEdge; j++) {
            const float x = maxWidth - static_cast<float>(dimension) * static_cast<float>(i) / static_cast<float>(divPerEdge);
            const float y = 0.0f;
            const float z = maxWidth - static_cast<float>(dimension) * static_cast<float>(j) / static_cast<float>(divPerEdge);
            pointGen.addPoint(x, y, z, 0.0f, 1.0f, 0.0f);

            int currPointIndex = pointsPerEdge * i + (j + 1);

            // north-facing triangles
            if (j < divPerEdge && i < divPerEdge) {
                pointGen.addAssociation(currPointIndex, currPointIndex + 1,
                    currPointIndex + pointsPerEdge);
            }
            // south-facing triangles
            if (j > 0 && i > 0) {
                pointGen.addAssociation(currPointIndex, currPointIndex - 1,
                    currPointIndex - pointsPerEdge);
            }
        }
    }

    FileWriter::writeToFile(filename, pointGen);
}
