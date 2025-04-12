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

void renderModelInCurve(const std::vector<GLuint>& buffers, const Model model);

void renderVBOModel(const std::vector<GLuint>& buffers, const Model model);

#endif
