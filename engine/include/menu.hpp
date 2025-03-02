#ifndef MENU_HPP
#define MENU_HPP

#include "xml_parser.hpp"

void getWindowSize(int w, int h);

void initializeImGUI(void);

void drawMenu(WorldConfig* worldconfig);

void shutdownMenu(void);

void renderDrawData(void);

#endif
