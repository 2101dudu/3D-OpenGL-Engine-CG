#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include "imgui.h"
#include <cstring>
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

    unsigned char tracking = 0;
    bool showInfoWindow = false;

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
    float up[3] = { 0.0f, 1.0f, 0.0f }; // initial up vector, refined by each alignment
};

struct Material {
    float diffuse[4];
    float ambient[4];
    float specular[4];
    float emissive[4];
    float shininess;

    bool operator==(Material const& o) const
    {
        return std::memcmp(diffuse, o.diffuse, sizeof(diffuse)) == 0
            && std::memcmp(ambient, o.ambient, sizeof(ambient)) == 0
            && std::memcmp(specular, o.specular, sizeof(specular)) == 0
            && std::memcmp(emissive, o.emissive, sizeof(emissive)) == 0
            && shininess == o.shininess;
    }
};

struct ModelCore {
    std::string file;
    int vboIndex = 0; // VBO id
    int iboIndex = 0; // IBO id
    size_t vertexCount = 0; // VBO vertice count
    size_t indexCount = 0; // IBO number count (3 × #triangles)
    size_t triangleCount = 0; // purely for stats

    bool operator==(const ModelCore& o) const
    {
        return file == o.file;
    }
};

struct Model {
    ModelCore* modelCore;
    Material material;
    std::string textureFilePath;
    int texIndex = 0; // texture id
    std::string filesModelsKey;

    bool operator==(const Model& o) const
    {
        return modelCore == o.modelCore
            && material == o.material
            && textureFilePath == o.textureFilePath;
    }
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
    bool drawAxis = false;
    bool drawCatmullRomCurves = false;
    bool lighting = true;
    bool textures = true;
    ImVec4 bgColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
};

struct Stats {
    int64_t numTriangles = 0;
};

enum class LightType { POINT,
    DIRECTIONAL,
    SPOTLIGHT };

struct LightConfig {
    LightType type;
    float position[4]; // w = 1 for point, 0 for directional
    float direction[3];
    float cutoff;
};

struct WorldConfig {
    WindowConfig window;
    CameraConfig camera;
    GroupConfig group;
    std::map<unsigned char, GroupConfig*> clickableGroups;
    std::map<std::string, Model*> filesModels;
    SceneConfig scene;
    Stats stats;
    std::vector<LightConfig> lights;
};

void resetCamera(WorldConfig* config);

void updateCamera(WorldConfig* config);

void switchCameraMode(WorldConfig* config);

void updateCameraLookAt(WorldConfig* config);

#endif
