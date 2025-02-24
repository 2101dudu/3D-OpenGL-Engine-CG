#define _USE_MATH_DEFINES
#include "draw.hpp"
#include "utils.hpp"
#include "xml_parser.hpp"
#include <iostream>
#include <math.h>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define MAX_MODELS 2

float cameraAngle = 90.0f;
float cameraAngleY = 0.0f;
WorldConfig config;

// VBOs
GLuint vertices, verticeCount;

void changeSize(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = w * 1.0f / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(config.camera.projection.fov, ratio, config.camera.projection.near, config.camera.projection.far);
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(5.0 * sin(cameraAngle), 5.0 * cos(cameraAngleY),
        5.0 * cos(cameraAngle), config.camera.lookAt.x, config.camera.lookAt.y, config.camera.lookAt.z,
        config.camera.up.x, config.camera.up.y, config.camera.up.z);

    drawAxis();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, verticeCount);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(config.camera.projection.fov, (GLfloat)w / (GLfloat)h, config.camera.projection.near, config.camera.projection.far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void processSpecialKeys(int key, int xx, int yy)
{
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

void initializeGLUT(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);

    // the winwow size is set in the main function
    // glutInitWindowSize(config.window.width, config.window.height);

    glutCreateWindow("CG@DI");

    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutSpecialFunc(processSpecialKeys);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config.xml>" << std::endl;
        return 1;
    }

    // GLUT needs to be initialized before initializing the VBOs
    initializeGLUT(argc, argv);

    // initialize VBOs
    glEnableClientState(GL_VERTEX_ARRAY);

    config = XMLParser::parseXML(argv[1]);
    XMLParser::configureFromXML(config);

    // now we can set the window size
    glutInitWindowSize(config.window.width, config.window.height);

    std::vector<float> modelPoints = parseFile(config.group.models[0].file.c_str());
    verticeCount = modelPoints.size();

    glGenBuffers(1, &vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * modelPoints.size(), modelPoints.data(), GL_STATIC_DRAW);

    glutMainLoop();

    return 1;
}
