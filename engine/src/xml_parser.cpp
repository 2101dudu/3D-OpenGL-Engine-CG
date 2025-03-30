#include "xml_parser.hpp"
#include "tinyxml2.h"
#include <cmath>
#include <iostream>

using namespace tinyxml2;

void parseGroup(XMLElement* groupElement, GroupConfig& group)
{
    // parse transformations if avilable
    XMLElement* transformElement = groupElement->FirstChildElement("transform");
    if (transformElement) {
        // Parse the rotate:
        XMLElement* rotateElement = transformElement->FirstChildElement("rotate");
        if (rotateElement) {
            Transform t;
            t.type = TransformType::Rotate;
            rotateElement->QueryFloatAttribute("angle", &t.angle);
            rotateElement->QueryFloatAttribute("x", &t.x);
            rotateElement->QueryFloatAttribute("y", &t.y);
            rotateElement->QueryFloatAttribute("z", &t.z);
            group.transforms.push_back(t);
        }

        // Parse the translate:
        XMLElement* translateElement = transformElement->FirstChildElement("translate");
        if (translateElement) {
            Transform t;
            t.type = TransformType::Translate;
            translateElement->QueryFloatAttribute("x", &t.x);
            translateElement->QueryFloatAttribute("y", &t.y);
            translateElement->QueryFloatAttribute("z", &t.z);
            group.transforms.push_back(t);
        }

        // Parse the scale:
        XMLElement* scaleElement = transformElement->FirstChildElement("scale");
        if (scaleElement) {
            Transform t;
            t.type = TransformType::Scale;
            scaleElement->QueryFloatAttribute("x", &t.x);
            scaleElement->QueryFloatAttribute("y", &t.y);
            scaleElement->QueryFloatAttribute("z", &t.z);
            group.transforms.push_back(t);
        }
    }

    // Parse <models> element.
    XMLElement* modelsElement = groupElement->FirstChildElement("models");
    if (modelsElement) {
        XMLElement* modelElement = modelsElement->FirstChildElement("model");
        while (modelElement) {
            Model modelConfig;
            if (modelElement->Attribute("file"))
                modelConfig.file = modelElement->Attribute("file");
            group.models.push_back(modelConfig);
            modelElement = modelElement->NextSiblingElement("model");
        }
    }

    // recursively parse nested child groups if available
    XMLElement* childGroupElement = groupElement->FirstChildElement("group");
    while (childGroupElement) {
        GroupConfig childGroup;
        parseGroup(childGroupElement, childGroup);
        group.children.push_back(childGroup);
        childGroupElement = childGroupElement->NextSiblingElement("group");
    }
}

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
        parseGroup(group, config.group);
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

    GroupConfig rootGroup = config.group;
}
