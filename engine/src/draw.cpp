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

void applyTransformations(const std::vector<Transform>& transforms)
{
    for (const auto& t : transforms) {
        switch (t.type) {
        case TransformType::Translate:
            glTranslatef(t.x, t.y, t.z);
            break;
        case TransformType::Rotate:
            glRotatef(t.angle, t.x, t.y, t.z);
            break;
        case TransformType::Scale:
            glScalef(t.x, t.y, t.z);
            break;
        }
    }
}

void drawWithVBOs(const std::vector<GLuint>& buffers, const GroupConfig& group)
{
    glPushMatrix();

    applyTransformations(group.transforms);

    glColor3f(group.color.x, group.color.y, group.color.z);

    // TODO: check if this line is necessary
    glEnableClientState(GL_VERTEX_ARRAY);

    for (const auto& model : group.models) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[model.vboIndex]);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for (const auto& subGroup : group.children) {
        drawWithVBOs(buffers, subGroup);
    }

    glPopMatrix();
}
