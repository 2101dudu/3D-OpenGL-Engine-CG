#include "FileWriter.hpp"
#include <iostream>

void FileWriter::writeToFile(const std::string& fileName, const PointsGenerator& generator) {
    std::ofstream file(fileName);
    if (!file) {
        std::cerr << "Error opening file '" << fileName << "' for writing." << std::endl;
        return;
    }

    file << generator.points.size() << "\n";
    for (const auto& p : generator.points) {
        file << p.x << " " << p.y << " " << p.z << "\n";
    }
    file << generator.associations.size() << "\n";
    for (const auto& a : generator.associations) {
        file << a.p1 << " " << a.p2 << " " << a.p3 << "\n";
    }

    file.close();
}
