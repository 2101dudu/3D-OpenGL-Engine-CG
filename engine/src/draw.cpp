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
#include <cmath>
#endif

#include "catmull_rom.hpp"
#include "draw.hpp"

extern float globalTimer;
extern float timeFactor;
extern bool drawCatmullRomCurves;

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
            if (t.curveTime > 0.0f) {
                float pos[3], deriv[3];

                if (drawCatmullRomCurves) {
                    glColor3f(1.0f, 1.0f, 0.0f);
                    glBegin(GL_LINE_LOOP);
                    for (float _t = 0; _t < 1; _t += 0.01f) {
                        getGlobalCatmullRomPoint(_t, pos, deriv, t);
                        glVertex3f(pos[0], pos[1], pos[2]);
                    }
                    glEnd();
                }

                getGlobalCatmullRomPoint(-1.0f, pos, deriv, t);

                glTranslatef(pos[0], pos[1], pos[2]);

                if (t.align) {
                    float forward[3] = { deriv[0], deriv[1], deriv[2] };
                    float* rotMatrix = getRotMatrix(forward);
                    glMultMatrixf(rotMatrix);
                    free(rotMatrix);
                }
            } else {
                glTranslatef(t.x, t.y, t.z);
            }
            break;
        case TransformType::Rotate:
            if (t.time > 0.0f) {
                int elapsedInCycle = fmod(globalTimer, t.time * 1000);

                float currRotation = (elapsedInCycle / (t.time * 1000.0f)) * 360.0f;
                glRotatef(currRotation, t.x, t.y, t.z);
            } else {
                glRotatef(t.angle, t.x, t.y, t.z);
            }
            break;
        case TransformType::Scale:
            glScalef(t.x, t.y, t.z);
            break;
        }
    }
}

void drawWithVBOs(const std::vector<GLuint>& vboBuffers,
                  const std::vector<GLuint>& iboBuffers,
                  const GroupConfig& group)
{
    glPushMatrix();
    applyTransformations(group.transforms);
    glEnableClientState(GL_VERTEX_ARRAY);

    for (const auto& model : group.models) {
        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, vboBuffers[model.vboIndex]);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        // IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboBuffers[model.iboIndex]);

        // Desenha
        glDrawElements(GL_TRIANGLES,
                       model.indexCount,
                       GL_UNSIGNED_INT,
                       0);

        // Opcional: desliga o ELEMENT_ARRAY
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Chamada recursiva — passa ambos os vetores
    for (const auto& child : group.children) {
        drawWithVBOs(vboBuffers, iboBuffers, child);
    }

    glPopMatrix();
}
