//
// Created by Eduardo Faria on 19/02/2025.
//

#include "sphere.hpp"

void Sphere::createSphere(int radius, int slices, int stacks) {
  PointsGenerator pointGen;

  // angle per slice
  float alpha = 2 * M_PI / slices;

  // angle per stack
  float beta =  M_PI / stacks;

  for (int i = 0; i < slices; i++) {
    float currAlpha = alpha * i;

    for (int j = 0; j < stacks; j++) {
      float currBeta = static_cast<float>(-M_PI/2) + beta * j;

      float x = cosf(currAlpha) * radius * cos(currBeta);
      float y = sinf(currBeta) * radius;
      float z = sinf(currAlpha) * radius * cos(currBeta);

      pointGen.addPoint(x, y, z);


      //int currPointIndex = pointsPerEdge * i + (j + 1);
//
      //// north-facing triangles
      //if (j < divPerEdge && i < divPerEdge) {
      //  pointGen.addAssociation(currPointIndex, currPointIndex + 1,
      //                          currPointIndex + pointsPerEdge);
      //}
      //// south-facing triangles
      //if (j > 0 && i > 0) {
      //  pointGen.addAssociation(currPointIndex, currPointIndex - 1,
      //                          currPointIndex - pointsPerEdge);
      //}
    }
  }

  pointGen.showData();

  FileWriter::writeToFile("pontos.txt", pointGen);
}
