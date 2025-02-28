#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP

#include "structs.h"
#include <string>

class XMLParser {
public:
    static WorldConfig parseXML(const std::string& filename);
    static void configureFromXML(const WorldConfig& config);
};

void resetCamera(WorldConfig* config);

#endif
