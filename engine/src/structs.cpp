#include "structs.hpp"

#include <cmath>

void resetCamera(WorldConfig* config)
{
    // cartesian
    config->camera.tracking = 1;
    config->camera.lookAt = { 0, 0, 0 };
    config->camera.up = { 0, 1, 0 };
    float zoomInCartesian = 50;

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

void switchCameraMode(WorldConfig* config)
{

    // Toggle camera mode
    config->camera.isOrbital = !config->camera.isOrbital;

    if (!config->camera.isOrbital) {
        config->camera.tracking = 0;
        config->camera.showInfoWindow = false;

        // Compute the vector from the camera position to the lookAt point
        float dx = config->camera.lookAt.x - config->camera.position.x;
        float dy = config->camera.lookAt.y - config->camera.position.y;
        float dz = config->camera.lookAt.z - config->camera.position.z;

        // Normalize the direction vector
        float length = sqrtf(dx * dx + dy * dy + dz * dz);
        if (length == 0.0f) {
            length = 1.0f;
        }
        float ndx = dx / length;
        float ndy = dy / length;
        float ndz = dz / length;

        config->camera.cameraAngle = atan2f(ndz, ndx);
        config->camera.cameraAngleY = asinf(ndy);
    } else {
        float dx = config->camera.position.x;
        float dy = config->camera.position.y;
        float dz = config->camera.position.z;
        float distance = sqrtf(dx * dx + dy * dy + dz * dz);
        config->camera.cameraDistance = distance;

        config->camera.cameraAngle = atan2f(dz, dx);
        config->camera.cameraAngleY = asinf(dy / distance);

        updateCameraLookAt(config);
    }
}

void updateCameraLookAt(WorldConfig* config)
{
    const unsigned char picked = config->camera.tracking;
    if (picked == 0)
        return;
    GroupConfig* g = config->clickableGroups[picked];
    config->camera.lookAt.x = g->center.x;
    config->camera.lookAt.y = g->center.y;
    config->camera.lookAt.z = g->center.z;

    updateCamera(config);
}
