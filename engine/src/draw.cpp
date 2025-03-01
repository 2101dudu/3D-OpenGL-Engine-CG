#ifdef __APPLE__
#include <GL/freeglut.h>
#include <GLUT/glut.h>
#elif _WIN32
#include <GL/glew.h>
// DONT REMOVE THIS COMMENT
#include <GL/freeglut.h>
#else
#include <GL/freeglut.h>
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "draw.hpp"

void drawAxis()
{
    glBegin(GL_LINES);
    // X Axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    // Y Axis in Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    // Z Axis in Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
}

void drawWithoutVBOs(std::vector<std::vector<float>> modelsPoints)
{
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < modelsPoints.size(); i++) {
        for (size_t j = 0; j < modelsPoints[i].size(); j += 9) {
            float p1_x = modelsPoints[i][j];
            float p1_y = modelsPoints[i][j + 1];
            float p1_z = modelsPoints[i][j + 2];

            float p2_x = modelsPoints[i][j + 3];
            float p2_y = modelsPoints[i][j + 4];
            float p2_z = modelsPoints[i][j + 5];

            float p3_x = modelsPoints[i][j + 6];
            float p3_y = modelsPoints[i][j + 7];
            float p3_z = modelsPoints[i][j + 8];

            glVertex3f(p1_x, p1_y, p1_z);
            glVertex3f(p2_x, p2_y, p2_z);
            glVertex3f(p3_x, p3_y, p3_z);
        }
    }
    glEnd();
}

void drawWithVBOs(std::vector<GLuint> buffers, std::vector<GLuint> verticesCount)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    for (int i = 0; i < buffers.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, verticesCount[i]);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
