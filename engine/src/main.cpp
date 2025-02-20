#define _USE_MATH_DEFINES
#include <chrono>
#include <math.h>
#include <sstream>
#include "draw.hpp"
#include "utils.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int frameCount = 0;
float fps = 0.0f;
auto lastTime = std::chrono::steady_clock::now();

float cameraAngle = 90.0f;
float cameraAngleY = 0.0f;
char* file = "../build/pontos.txt";
std::vector<Point> points;

void displayFps() {
    // FPS Calculation
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

    if (deltaTime >= 1.0f) { // Every second
        fps = frameCount / deltaTime;
        frameCount = 0;
        lastTime = currentTime;
    }

    // Render FPS text
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 800); // Match window size
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    std::ostringstream fpsText;
    fpsText << "FPS: " << (int)fps;
    std::string fpsStr = fpsText.str();

    glColor3f(1.0f, 1.0f, 1.0f); // White color
    glRasterPos2f(10, 780); // Position on the screen

    for (char c : fpsStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

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

    displayFps();

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