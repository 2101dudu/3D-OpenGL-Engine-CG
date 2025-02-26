#ifndef MENU_HPP
#define MENU_HPP

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

void getWindowSize(int w, int h);

void initializeImGUI(void);

void drawMenu(void);

void shutdownMenu(void);

void renderDrawData(void);

#endif
