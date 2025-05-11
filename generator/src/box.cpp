#include "box.hpp"

void Box::createBox(int dimension, int divPerEdge, const std::string& filename)
{
    PointsGenerator pointGen;

    // the interval specified by the dimension, i.e.,
    // dimension = 2 --> [-1,1]
    const float maxWidth = dimension / 2;

    // the number of unique points per number of divisions,
    // i.e., 3 divisions means 4 points
    const int pointsPerEdge = divPerEdge + 1;

    // iterate over the 6 possible normals' values
    // (u, v, w)
    // (0, 0, 1)
    // (0, 0, -1)
    // (0, 1, 0)
    // (0, -1, 0)
    // (1, 0, 0)
    // (-1, 0, 0)
    for (int k = 0; k < 6; k++) {
        int u = 0;
        int v = 0;
        int w = 0;
        switch (k / 2) {
        case 0:
            w = 1 - 2 * (k % 2);
            break;
        case 1:
            v = 1 - 2 * (k % 2);
            break;
        case 2:
            u = 1 - 2 * (k % 2);
            break;
        }

        for (int i = 0; i < pointsPerEdge; i++) {
            for (int j = 0; j < pointsPerEdge; j++) {
                // translation according to the dimension of the box
                const float dx = maxWidth * u;
                const float dy = maxWidth * v;
                const float dz = maxWidth * w;

                // check if an axis corresponds to the normal
                const int isXNormal = abs(u);
                const int isYNormal = abs(v);
                const int isZNormal = abs(w);

                // if the point aligns with the axis bound to the normal,
                // its value shall be determined only by the translation amount.
                // otherwise, a normal coordinate algorithm shall pursue
                const float x = isXNormal ? dx : maxWidth - 2.0f * i / divPerEdge;
                const float y = isYNormal ? dy : maxWidth - 2.0f * (i * isXNormal + j * isZNormal) / divPerEdge;
                const float z = isZNormal ? dz : maxWidth - 2.0f * j / divPerEdge;

                pointGen.addPoint(x, y, z, u, v, w);

                int currPointIndex = k * pointsPerEdge * pointsPerEdge + i * pointsPerEdge + j + 1;

                // Determine if we need to swap winding order
                bool swapWinding = (w == 1 || v == -1 || u == 1);

                // North-facing triangles (lower-left triangle)
                if (j < divPerEdge && i < divPerEdge) {
                    int a = currPointIndex;
                    int b = currPointIndex + 1;
                    int c = currPointIndex + pointsPerEdge;
                    pointGen.addAssociation(a, swapWinding ? c : b, swapWinding ? b : c);
                }

                // South-facing triangles (upper-right triangle)
                if (j > 0 && i > 0) {
                    int a = currPointIndex;
                    int b = currPointIndex - 1;
                    int c = currPointIndex - pointsPerEdge;
                    pointGen.addAssociation(a, swapWinding ? c : b, swapWinding ? b : c);
                }
            }
        }
    }
    FileWriter::writeToFile(filename, pointGen);
}
