#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include "imgui.h"
#include <vector>

struct WindowConfig {
    int width;
    int height;
};

struct CameraConfig {
    struct Position {
        float x, y, z;
    } position, lookAt, up;
    struct Projection {
        float fov, near1, far1;
    } projection;

    float cameraAngle;
    float cameraAngleY;
    bool isDragging = false;
    int lastX, lastY;

    float cameraDistance;
    float sensitivity = 0.005;
    float scrollSensitivity = 0.05f;
};

struct ModelConfig {
    std::string file;
};

struct GroupConfig {
    std::vector<ModelConfig> models;

    ImVec4 color = ImVec4(0.78125f, 0.78125f, 0.78125f, 1.0f); // off-white color
};

struct WorldConfig {
    WindowConfig window;
    CameraConfig camera;
    GroupConfig group;

    struct SceneConfig {
        bool faceCulling = true;
        bool wireframe = true;
        bool drawAxis = true;
        bool useVBOs = true;

        ImVec4 bgColor = ImVec4(0.15625f, 0.15625f, 0.15625f, 1.0f); // off-black color
    } scene;

    struct Stats {
        int fps;
        int64_t numTriangles = 0;
    } stats;
};

#endif
