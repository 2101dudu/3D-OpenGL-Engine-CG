#include "PointsGenerator.hpp"
#include "FileWriter.hpp"
#include "box.hpp"

int main() {
    PointsGenerator generator;

    //generator.addPoint(0.0, 1.0, 0.0);
    //generator.addPoint(-1.0, -1.0, 1.0);
    //generator.addPoint(1.0, -1.0, 1.0);
    //generator.addPoint(-1.0, -1.0, -1.0);
    //generator.addPoint(1.0, -1.0, -1.0);

    //generator.addAssociation(1, 2, 3);
    //generator.addAssociation(2, 4, 5);

    Box::createBox(2, 3);


    //FileWriter::writeToFile("pontos.txt", generator);

    return 0;
}
