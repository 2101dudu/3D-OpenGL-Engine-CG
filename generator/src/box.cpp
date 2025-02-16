//
// Created by Eduardo Faria on 15/02/2025.
//


#include "box.hpp"

void Box::createBox(int dimension, int divPerEdge) {
    PointsGenerator pointGen;

    // the interval specified by the dimension, i.e.,
    // dimension = 2 --> [-1,1]
    const float maxWidth = dimension/2;

    // the number of unique points per number of divisions,
    // i.e., 3 divisions means 4 points
    const int pointsPerEdge = divPerEdge + 1;

    for (int i=0; i<pointsPerEdge; i++) {
        for (int j=0; j<pointsPerEdge; j++) {
            const float x = maxWidth - 2.0f*i/divPerEdge;
            const float y = 0.0f;
            const float z = maxWidth - 2.0f*j/divPerEdge;
            pointGen.addPoint(x, y, z);

            int currPointIndex = pointsPerEdge * i + (j + 1);

            // north-facing triangles
            if (j<3 && i<3) {
                pointGen.addAssociation(currPointIndex, currPointIndex + 1, currPointIndex + 4);
            }
            // south-facing triangles
            if (j>0 && i>0) {
                pointGen.addAssociation(currPointIndex, currPointIndex - 1, currPointIndex - 4);
            }
        }
    }

    pointGen.showData();
}