#include "xml_parser.hpp"
#include "tinyxml2.h"
#include <cmath>
#include <iostream>

using namespace tinyxml2;

WorldConfig XMLParser::parseXML(const std::string& filename)
{
    WorldConfig config;
    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load XML file: " << filename << std::endl;
        return config;
    }

    XMLElement* world = doc.FirstChildElement("world");
    if (!world)
        return config;

    XMLElement* window = world->FirstChildElement("window");
    if (window) {
        window->QueryIntAttribute("width", &config.window.width);
        window->QueryIntAttribute("height", &config.window.height);
    }

    XMLElement* camera = world->FirstChildElement("camera");
    if (camera) {
        XMLElement* position = camera->FirstChildElement("position");
        if (position) {
            position->QueryFloatAttribute("x", &config.camera.position.x);
            position->QueryFloatAttribute("y", &config.camera.position.y);
            position->QueryFloatAttribute("z", &config.camera.position.z);
        }
        XMLElement* lookAt = camera->FirstChildElement("lookAt");
        if (lookAt) {
            lookAt->QueryFloatAttribute("x", &config.camera.lookAt.x);
            lookAt->QueryFloatAttribute("y", &config.camera.lookAt.y);
            lookAt->QueryFloatAttribute("z", &config.camera.lookAt.z);
        }
        XMLElement* up = camera->FirstChildElement("up");
        if (up) {
            up->QueryFloatAttribute("x", &config.camera.up.x);
            up->QueryFloatAttribute("y", &config.camera.up.y);
            up->QueryFloatAttribute("z", &config.camera.up.z);
        }
        XMLElement* projection = camera->FirstChildElement("projection");
        if (projection) {
            projection->QueryFloatAttribute("fov", &config.camera.projection.fov);
            projection->QueryFloatAttribute("near", &config.camera.projection.near1);
            projection->QueryFloatAttribute("far", &config.camera.projection.far1);
        }
    }

    XMLElement* group = world->FirstChildElement("group");
    if (group) {
        XMLElement* models = group->FirstChildElement("models");
        if (models) {
            XMLElement* model = models->FirstChildElement("model");
            while (model) {
                ModelConfig modelConfig;
                modelConfig.file = model->Attribute("file");
                config.group.models.push_back(modelConfig);
                model = model->NextSiblingElement("model");
            }
        }
    }

    return config;
}

void calculateSphericalCoordinates(float x, float y, float z, float& alpha, float& beta, float& radius)
{
    radius = std::sqrt(x * x + y * y + z * z);
    alpha = std::atan2(z, x);
    beta = std::asin(y / radius);
}

void XMLParser::configureFromXML(WorldConfig& config)
{
    std::cout << "World configuration:" << std::endl;
    std::cout << "Window width: " << config.window.width << ", height: " << config.window.height << std::endl;
    std::cout << "Camera position: (" << config.camera.position.x << ", " << config.camera.position.y << ", " << config.camera.position.z << ")" << std::endl;
    std::cout << "Camera lookAt: (" << config.camera.lookAt.x << ", " << config.camera.lookAt.y << ", " << config.camera.lookAt.z << ")" << std::endl;
    std::cout << "Camera up: (" << config.camera.up.x << ", " << config.camera.up.y << ", " << config.camera.up.z << ")" << std::endl;
    std::cout << "Camera projection: fov=" << config.camera.projection.fov << ", near=" << config.camera.projection.near1 << ", far=" << config.camera.projection.far1 << std::endl;

    float alpha, beta, radius;
    calculateSphericalCoordinates(config.camera.position.x, config.camera.position.y, config.camera.position.z,
        alpha, beta, radius);

    config.camera.cameraAngle = alpha;
    config.camera.cameraAngleY = beta;
    config.camera.cameraDistance = radius;

    for (const auto& model : config.group.models) {
        std::cout << "Model file: " << model.file << std::endl;
    }
}
