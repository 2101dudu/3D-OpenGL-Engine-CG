#ifndef DRAW_HPP
#define DRAW_HPP

#include "structs.hpp"
#include "vector"

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

void drawAxis();

void drawWithoutVBOs(const GroupConfig& group);

void drawWithVBOs(const std::vector<GLuint>& buffers, const GroupConfig& group);

#endif
