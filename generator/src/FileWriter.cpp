// FileWriter.cpp
#include "FileWriter.hpp"
#include <iostream>

void FileWriter::writeToFile(const std::string& fileName, const PointsGenerator& generator) {
    std::ofstream file(fileName);
    if (!file) {
        std::cerr << "Error opening file '" << fileName << "' for writing." << std::endl;
        return;
    }

    // Write vertices with normals
    file << generator.getPoints().size() << "\n";
    for (const auto& p : generator.getPoints()) {
        file << p.x << " " << p.y << " " << p.z
             << " " << p.nx << " " << p.ny << " " << p.nz
                << " " << p.tx << " " << p.ty << "\n";
    }

    // Write associations (triangles)
    file << generator.getAssociations().size() << "\n";
    for (const auto& a : generator.getAssociations()) {
        file << a.p1 << " " << a.p2 << " " << a.p3 << "\n";
    }

    file.close();
}