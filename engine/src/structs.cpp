#include "structs.h"

void resetCamera(WorldConfig* config)
{
    config->camera.cameraAngle = 45.0f;
    config->camera.cameraAngleY = 0.0f;
    config->camera.cameraDistance = 5.0f;

    config->camera.position = { 4.255, 5, 2.267 };
    config->camera.lookAt = { 0, 0, 0 };
    config->camera.up = { 0, 1, 0 };

    config->camera.sensitivity = 0.005;
    config->camera.scrollSensitivity = 0.05f;
}
