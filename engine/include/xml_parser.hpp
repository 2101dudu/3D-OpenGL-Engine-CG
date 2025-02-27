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
};

class XMLParser {
public:
    static WorldConfig parseXML(const std::string& filename);
    static void configureFromXML(const WorldConfig& config);
};

#endif
