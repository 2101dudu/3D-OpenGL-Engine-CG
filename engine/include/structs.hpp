#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include "imgui.h"
#include <string>
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

    bool isOrbital = true;
};

enum class TransformType {
    Translate,
    Rotate,
    Scale
};

struct Transform {
    TransformType type; // New member to identify the type of transformation
    // The values are used according to the type:
    // - For translate: uses x, y, z.
    // - For rotate: uses angle, x, y, z.
    // - For scale: uses x, y, z.
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float angle = 0.0f; // Only used for rotation
};

struct Model {
    std::string file;
    int vboIndex = 0; // VBO id
    size_t vertexCount = 0; // VBO vertice count
};

struct GroupConfig {
    ImVec4 color = ImVec4(0.78125f, 0.78125f, 0.78125f, 1.0f); // off-white color
    std::vector<Transform> transforms;
    std::vector<Model> models;
    std::vector<GroupConfig> children;
};

struct SceneConfig {
    bool faceCulling = true;
    bool wireframe = true;
    bool drawAxis = true;
    ImVec4 bgColor = ImVec4(0.15625f, 0.15625f, 0.15625f, 1.0f); // off-black color
};

struct Stats {
    int fps;
    int64_t numTriangles = 0;
};

struct WorldConfig {
    WindowConfig window;
    CameraConfig camera;
    GroupConfig group;
    SceneConfig scene;
    Stats stats;
};

void resetCamera(WorldConfig* config);

void updateCamera(WorldConfig* config);

void switchCameraMode(WorldConfig* config);

#endif
