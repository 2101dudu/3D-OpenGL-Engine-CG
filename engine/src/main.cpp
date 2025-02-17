#define _USE_MATH_DEFINES
#include <math.h>
#include "draw.hpp"
#include "utils.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

float cameraAngle = 90.0f;
float cameraAngleY = 0.0f;
char* file = "../build/pontos.txt";
std::vector<Point> points;

void changeSize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = w * 1.0f / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 1.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(5.0 * sin(cameraAngle), 5.0 * cos(cameraAngleY),
              5.0 * cos(cameraAngle), 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);

	drawAxis();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawTriangles(points);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void processSpecialKeys(int key, int xx, int yy) {
    switch (key) {
        case GLUT_KEY_LEFT:
            cameraAngle -= 0.1f;
            break;
        case GLUT_KEY_RIGHT:
            cameraAngle += 0.1f;
            break;
        case GLUT_KEY_UP:
            cameraAngleY += 0.1f;
            break;
        case GLUT_KEY_DOWN:
            cameraAngleY -= 0.1f;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {

	points = parseFile(file);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG@DI");

    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutSpecialFunc(processSpecialKeys);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glutMainLoop();

    return 1;
}