#define _USE_MATH_DEFINES
#include "draw.hpp"
#include "utils.hpp"
#include "xml_parser.hpp"
#include <iostream>
#include <math.h>

#ifdef __APPLE__
#include <GL/freeglut.h>
#include <GLUT/glut.h>
#elif _WIN32
#include <GL/glew.h>
#include <GL/freeglut.h>
#else
#include <GL/freeglut.h>
#include <GL/glew.h>
#include <GL/glut.h>
#endif

// Camera
float cameraAngle = 90.0f;
float cameraAngleY = 0.0f;
bool isDragging = false;
int lastX, lastY;
float cameraDistance = 5.0f; // initial value
float sensitivity = 0.005f;
float scrollSensitivity = 0.05f;

WorldConfig config;

// FPS
int timebase;
int frames = 0;

// VBOs
std::vector<GLuint> buffers;
std::vector<GLuint> verticesCount;

void changeSize(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = w * 1.0f / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(config.camera.projection.fov, ratio, config.camera.projection.near1, config.camera.projection.far1);
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(cameraDistance * sin(cameraAngle), cameraDistance * cos(cameraAngleY), cameraDistance * cos(cameraAngle),
        config.camera.lookAt.x, config.camera.lookAt.y, config.camera.lookAt.z,
        config.camera.up.x, config.camera.up.y, config.camera.up.z);

    drawAxis();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    frames++;

    int time = glutGet(GLUT_ELAPSED_TIME);
    static float fps = 0.0f;

    if (time - timebase > 1000) {
        fps = frames * 1000.0f / (time - timebase);
        timebase = time;
        frames = 0;
    }

    char buf[10];
    snprintf(buf, sizeof(buf), "%.1f", fps);
    glutSetWindowTitle(buf);

    for (int i = 0; i < buffers.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, verticesCount[i]);
    }

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(config.camera.projection.fov, (GLfloat)w / (GLfloat)h, config.camera.projection.near1, config.camera.projection.far1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// track mouse button presses
void mouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastX = x;
            lastY = y;
        } else {
            isDragging = false;
        }
    }
}

// track mouse movement
void mouseMotion(int x, int y)
{
    if (isDragging) {
        float dx = (lastX - x) * sensitivity; // horizontal movement reversed for better feel
        float dy = (y - lastY) * sensitivity;

        cameraAngle += dx;
        cameraAngleY += dy;

        lastX = x;
        lastY = y;

        glutPostRedisplay();
    }
}

// handle scroll wheel zooming (FreeGLUT ONLY)
void mouseWheel(int wheel, int direction, int x, int y)
{
    if (direction > 0 && cameraDistance > 1.0f) {
        // scroll in
        cameraDistance -= scrollSensitivity;
        glutPostRedisplay();
    } else if (direction < 0) {
        // scroll down
        cameraDistance += scrollSensitivity;
        glutPostRedisplay();
    }
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

void initializeGLUTPreWindow(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
}

WorldConfig loadConfiguration(const char* configFile)
{
    WorldConfig cfg = XMLParser::parseXML(configFile);
    XMLParser::configureFromXML(cfg);
    return cfg;
}

void createWindowWithConfig(const WorldConfig& cfg)
{
    glutInitWindowSize(cfg.window.width, cfg.window.height);
    glutCreateWindow("CG@DI");
}

void setupCallbacks()
{
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutMouseWheelFunc(mouseWheel); // wheel function
}

void initializeOpenGLContext()
{
#ifndef __APPLE__
    glewInit();
#endif
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void initializeVBOs()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    int n = config.group.models.size();
    buffers.resize(n);
    verticesCount.resize(n);
    glGenBuffers(n, buffers.data());
    int counter = 0;
    for (const auto& model : config.group.models) {
        std::vector<float> modelPoints = parseFile(model.file.c_str());
        verticesCount[counter] = modelPoints.size() / 3;
        glBindBuffer(GL_ARRAY_BUFFER, buffers[counter]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * modelPoints.size(),
            modelPoints.data(), GL_STATIC_DRAW);
        counter++;
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config.xml>" << std::endl;
        return 1;
    }

    // pre-window initialization: Setup GLUT and load configuration
    initializeGLUTPreWindow(argc, argv);
    config = loadConfiguration(argv[1]);

    // create the window using configuration parameters
    createWindowWithConfig(config);

    // setup callbacks and initialize the OpenGL context
    setupCallbacks();
    initializeOpenGLContext();

    // initialize VBOs
    initializeVBOs();

    // enter the GLUT main loop
    glutMainLoop();
    return 1;
}
