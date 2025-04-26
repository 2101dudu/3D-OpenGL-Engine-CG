#include "Bezier.hpp"
#include "FileWriter.hpp"
#include "PointsGenerator.hpp"
#include "box.hpp"
#include "cone.hpp"
#include "cylinder.hpp"
#include "plane.hpp"
#include "sphere.hpp"
#include "torus.hpp"

#include <string>

void executeCommand(const std::string& command, const std::vector<int>& args, const std::string& filename, int argc, char* argv[])
{
    std::string project_filename = "../../objects/" + filename;

    if (command == "sphere" && args.size() == 3) {
        Sphere::createSphere(args[0], args[1], args[2], project_filename);
    } else if (command == "box" && args.size() == 2) {
        Box::createBox(args[0], args[1], project_filename);
    } else if (command == "plane" && args.size() == 2) {
        Plane::createPlane(args[0], args[1], project_filename);
    } else if (command == "cone" && args.size() == 4) {
        Cone::createCone(args[0], args[1], args[2], args[3], project_filename);
    } else if (command == "torus" && args.size() == 4) {
        Torus::createTorus(args[0], args[1], args[2], args[3], project_filename);
    } else if (command == "cylinder" && args.size() == 4) {
        Cylinder::createCylinder(args[0], args[1], args[2], args[3], project_filename);
    } else if (command == "patch" && args.size() == 1 && argc >= 5) {
        std::string inputPatchFile = argv[argc - 3];
        std::string outputFile = "../../objects/" + std::string(argv[argc - 1]);
        Bezier::createBezierModel(inputPatchFile, args[0], outputFile);
    } else {
        std::cerr << "Unknown command or incorrect parameters: " << command << std::endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 5) {
        std::cerr << "Usage: generator <command> <param1> <param2> ...\n";
        return 1;
    }

    std::string command = argv[1];
    std::vector<int> args;

    if (command == "patch") {
        try {
            args.push_back(std::stoi(argv[3]));
        } catch (...) {
            std::cerr << "Error: tesselation parameter is invalid for bezier.\n";
            return 1;
        }
    } else {
        for (int i = 2; i < argc - 1; ++i) {
            try {
                args.push_back(std::stoi(argv[i]));
            } catch (...) {
                std::cerr << "Error: invalid argument for the command: " << command << ".\n";
                return 1;
            }
        }
    }

    executeCommand(command, args, argv[argc - 1], argc, argv);
    return 0;
}
