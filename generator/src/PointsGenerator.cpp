#include "PointsGenerator.hpp"

void PointsGenerator::addPoint(float x, float y, float z) {
    points.push_back((Point) {x, y, z});
}

void PointsGenerator::addAssociation(int p1, int p2, int p3) {
    associations.push_back((Association) {p1, p2, p3});
}

void PointsGenerator::showData() const {
    std::cout << points.size() << "\n";
    for (const auto& p : points) {
        std::cout << p.x << " " << p.y << " " << p.z << "\n";
    }
    std::cout << associations.size() << "\n";
    for (const auto& a : associations) {
        std::cout << a.p1 << " " << a.p2 << " " << a.p3 << "\n";
    }
}
