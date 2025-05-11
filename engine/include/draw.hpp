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

void drawWithVBOs(const std::vector<GLuint>& vboBuffers,
                  const std::vector<GLuint>& vboBuffersNormals,
                  const std::vector<GLuint>& iboBuffers,
                  GroupConfig& group,
                  bool depthOnly);

#endif
