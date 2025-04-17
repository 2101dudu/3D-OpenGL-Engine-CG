#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include "imgui.h"
#include <map>
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

    unsigned char tracking = 1;

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
    // - For translate: uses <time> x, y, z.
    // - For rotate: uses <time/angle>, x, y, z.
    // - For scale: uses x, y, z.
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float angle = 0.0f; // Only used for rotation

    // rotation animation
    float time = 0.0f;
    float currentRotation = 0.0f; // accumulated rotation angle (in degrees)

    // catmull rom
    size_t numberCurvePoints = 0;
    float** curvePoints;

    float curveTime = 0.0f;
    bool align = false;
};

struct Model {
    std::string file;
    int vboIndex = 0; // VBO id
    size_t vertexCount = 0; // VBO vertice count
    size_t triangleCount = 0; // purely for stats
};

struct GroupConfig {
    ImVec4 color = ImVec4(0.78125f, 0.78125f, 0.78125f, 1.0f); // off-white color
    std::vector<Transform> transforms;
    std::vector<Model*> models;
    std::vector<GroupConfig*> children;

    unsigned char id;
    std::string name;
    std::string infoText;
    struct Position {
        float x = 0, y = 0, z = 0;
    } center;
};

struct SceneConfig {
    bool faceCulling = true;
    bool wireframe = false;
    bool drawAxis = true;
    bool drawCatmullRomCurves = false;
    ImVec4 bgColor = ImVec4(0.15625f, 0.15625f, 0.15625f, 1.0f); // off-black color
};

struct Stats {
    int64_t numTriangles = 0;
};

struct WorldConfig {
    WindowConfig window;
    CameraConfig camera;
    GroupConfig group;
    std::map<unsigned char, GroupConfig*> clickableGroups;
    std::map<std::string, Model*> filesModels;
    SceneConfig scene;
    Stats stats;
};

void resetCamera(WorldConfig* config);

void updateCamera(WorldConfig* config);

void switchCameraMode(WorldConfig* config);

void updateCameraLookAt(WorldConfig* config);

#endif
