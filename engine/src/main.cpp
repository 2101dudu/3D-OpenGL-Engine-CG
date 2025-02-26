#define _USE_MATH_DEFINES
#include "draw.hpp"
#include "utils.hpp"
#include "xml_parser.hpp"
#include <iostream>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

float cameraAngle = 90.0f;
float cameraAngleY = 0.0f;
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

    frames++;

    int time = glutGet(GLUT_ELAPSED_TIME);
    static float fps = 0.0f; // Ensure fps has a valid initial value

    if (time - timebase > 1000) {
        fps = frames * 1000.0f / (time - timebase);
        timebase = time;
        frames = 0;
    }

    char buf[10]; // Allocate memory for the string
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
