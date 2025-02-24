#include "FileWriter.hpp"
#include "PointsGenerator.hpp"
#include "box.hpp"
#include "cylinder.hpp"
#include "cone.hpp"
#include "plane.hpp"
#include "sphere.hpp"
#include "torus.hpp"

#include <string>

void executeCommand(const std::string& command, const std::vector<int>& args, const std::string& filename)
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
    } else {
        std::cerr << "Unknown command or incorrect parameters: " << command << std::endl;
    }
}

int main(int argc, char* argv[])
{
    // minimum: "generator command param1 param2 ..."
    if (argc < 5) {
        std::cerr << "Usage: generator <command> <param1> <param2> ...\n";
        return 1;
    }

    // first argument is the command
    std::string command = argv[1];
    std::vector<int> args;

    // convert parameters to integers
    for (int i = 2; i < argc - 1; ++i) {
        args.push_back(std::stoi(argv[i]));
    }

    executeCommand(command, args, argv[argc - 1]);

    return 0;
}
