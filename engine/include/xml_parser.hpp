#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP

#include "structs.hpp"
#include <string>

class XMLParser {
public:
    static WorldConfig parseXML(const std::string& filename);
    static void configureFromXML(WorldConfig& config);
};

#endif
