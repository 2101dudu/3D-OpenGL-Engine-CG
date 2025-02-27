#include <OpenGL/gl.h>
#define _USE_MATH_DEFINES
#include "draw.hpp"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include "menu.hpp"
#include "utils.hpp"
#include "xml_parser.hpp"
#include <iostream>
#include <math.h>

#ifdef __APPLE__
#include <GL/freeglut.h>
#include <GLUT/glut.h>
#elif _WIN32
#include <GL/freeglut.h>
#include <GL/glew.h>
#else
#include <GL/freeglut.h>
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define FPS_UPDATE_TIME_MS 200

// Camera
float cameraAngle = 90.0f;
float cameraAngleY = 0.0f;
bool isDragging = false;
int lastX, lastY;

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

void updateSceneOptions(void)
{
    uint mode = config.scene.wireframe ? GL_LINE : GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, mode);

    if (config.scene.faceCulling) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(config.camera.cameraDistance * sin(cameraAngle), config.camera.cameraDistance * cos(cameraAngleY), config.camera.cameraDistance * cos(cameraAngle),
        config.camera.lookAt.x, config.camera.lookAt.y, config.camera.lookAt.z,
        config.camera.up.x, config.camera.up.y, config.camera.up.z);

    drawAxis();

    glEnableClientState(GL_VERTEX_ARRAY);
    for (int i = 0; i < buffers.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, verticesCount[i]);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    drawMenu(&config);

    // update scene options based on the menu
    updateSceneOptions();

    // FPS
    frames++;
    int time = glutGet(GLUT_ELAPSED_TIME);
    static float fps = 0.0f;
    if (time - timebase > FPS_UPDATE_TIME_MS) {
        fps = frames * 1000.0f / (time - timebase);
        timebase = time;
        frames = 0;
    }
    config.stats.fps = fps;

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
    if (h == 0)
        h = 1;
    // Update viewport and projection
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
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((float)x, (float)y);

    // check for scroll events
    if (button == 3 || button == 4) {
        // convert button 3 (scroll up) and 4 (scroll down) to wheel events
        float wheelDelta = (button == 3) ? 1.0f : -1.0f;
        io.AddMouseWheelEvent(0.0f, wheelDelta);

        if (!io.WantCaptureMouse) {
            if (wheelDelta > 0 && config.camera.cameraDistance > 1.0f)
                config.camera.cameraDistance -= config.camera.scrollSensitivity;
            else if (wheelDelta < 0)
                config.camera.cameraDistance += config.camera.scrollSensitivity;
            glutPostRedisplay();
        }
        // no need to progress
        return;
    }

    // this line repeats the assert in ImGUI's source code
    if (button >= 0 && button < ImGuiMouseButton_COUNT) {
        io.AddMouseButtonEvent(button, state == GLUT_DOWN);
    }

    // click and drag
    if (!io.WantCaptureMouse) {
        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_DOWN) {
                isDragging = true;
                lastX = x;
                lastY = y;
            } else if (state == GLUT_UP) {
                isDragging = false;
            }
        }
    }
}

// track mouse movement
void mouseMotion(int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);

    if (!io.WantCaptureMouse) {
        if (isDragging) {
            float dx = (lastX - x) * config.camera.sensitivity; // horizontal movement reversed for better feel
            float dy = (y - lastY) * config.camera.sensitivity;

            cameraAngle += dx;
            cameraAngleY += dy;

            lastX = x;
            lastY = y;

            glutPostRedisplay();
        }
    }
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
    glutIdleFunc(renderScene);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
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
        ModelInfo modelInfo = parseFile(model.file.c_str());
        config.stats.numTriangles += modelInfo.numTriangles;

        // deal with points
        std::vector<float> modelPoints = modelInfo.points;
        verticesCount[counter] = modelPoints.size() / 3;
        glBindBuffer(GL_ARRAY_BUFFER, buffers[counter]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * modelPoints.size(),
            modelPoints.data(), GL_STATIC_DRAW);
        counter++;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    // initialize the OpenGL context
    initializeOpenGLContext();

    // initialize VBOs
    initializeVBOs();

    // initialize menu
    initializeImGUI();

    // setup callbacks
    setupCallbacks();

    // enter the GLUT main loop
    glutMainLoop();

    shutdownMenu();

    return 1;
}
