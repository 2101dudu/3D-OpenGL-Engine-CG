#ifndef FileWriter_HPP
#define FileWriter_HPP

#include "PointsGenerator.hpp"
#include <fstream>

class FileWriter {
public:
    // Write the points and associations to a file in the format:
    // <number of points>
    // <x1> <y1> <z1>
    // <x2> <y2> <z2>
    // ...
    // <number of associations>
    // <p1> <p2> <p3>
    // <p1> <p2> <p3>
    // ...
    static void writeToFile(const std::string& fileName, const PointsGenerator& generator);
};

#endif
