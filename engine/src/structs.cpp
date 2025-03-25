#include "structs.hpp"

#include <cmath>

void resetCamera(WorldConfig* config)
{
    // cartesian
    config->camera.position = { 5, 5, 5 };
    config->camera.lookAt = { 0, 0, 0 };
    config->camera.up = { 0, 1, 0 };
    float zoomInCartesian = 5;

    // translate to spherical
    float distance = std::sqrt(zoomInCartesian * 5.0f + zoomInCartesian * 5.0f + zoomInCartesian * 5.0f);
    config->camera.cameraDistance = distance;
    config->camera.cameraAngle = std::atan2(5.0f, 5.0f);
    config->camera.cameraAngleY = std::asin(5.0f / distance);

    config->camera.sensitivity = 0.005;
    config->camera.scrollSensitivity = 0.05f;
}

void updateCamera(WorldConfig* config)
{
    config->camera.position = {
        config->camera.lookAt.x + config->camera.cameraDistance * cos(config->camera.cameraAngleY) * cos(config->camera.cameraAngle),
        config->camera.lookAt.y + config->camera.cameraDistance * sin(config->camera.cameraAngleY),
        config->camera.lookAt.z + config->camera.cameraDistance * cos(config->camera.cameraAngleY) * sin(config->camera.cameraAngle)
    };
}
