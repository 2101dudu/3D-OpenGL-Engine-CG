#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP

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

    float cameraDistance
        = 5.0f;
    float sensitivity = 0.005f;
    float scrollSensitivity = 0.05f;
};

struct ModelConfig {
    std::string file;
};

struct GroupConfig {
    std::vector<ModelConfig> models;
};

struct WorldConfig {
    WindowConfig window;
    CameraConfig camera;
    GroupConfig group;

    struct SceneConfig {
        bool faceCulling = false;
        bool wireframe = true;
    } scene;

    struct Stats {
        int fps;
        int64_t numTriangles = 0;
    } stats;
};

class XMLParser {
public:
    static WorldConfig parseXML(const std::string& filename);
    static void configureFromXML(const WorldConfig& config);
};

#endif
