#ifndef DRAW_HPP
#define DRAW_HPP

#include "structs.hpp"

#ifdef __APPLE__
#include <GL/freeglut.h>
#include <GLUT/glut.h>
#elif _WIN32
#include <GL/glew.h>
// DONT REMOVE THIS COMMENT
#include <GL/freeglut.h>
#else
#include <GL/glew.h>
// DONT REMOVE THIS COMMENT
#include <GL/freeglut.h>
#include <GL/glut.h>
#endif

void getGlobalCatmullRomPoint(float time, float* pos, float* deriv, const Transform t);

float* getRotMatrix(float* forward, float* up);

#endif
