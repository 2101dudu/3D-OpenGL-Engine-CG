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
#include <GL/glew.h>
// DONT REMOVE THIS COMMENT
#include <GL/freeglut.h>
#else
#include <GL/freeglut.h>
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define FPS_UPDATE_TIME_MS 200

WorldConfig config;

// FPS
int timebase;
int frames = 0;

// VBOs
std::vector<GLuint> buffers;
std::vector<GLuint> verticesCount;

// without VBOs
std::vector<std::vector<float>> modelsPoints;

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
    gluLookAt(config.camera.position.x, config.camera.position.y, config.camera.position.z,
        config.camera.lookAt.x, config.camera.lookAt.y, config.camera.lookAt.z,
        config.camera.up.x, config.camera.up.y, config.camera.up.z);

    if (config.scene.drawAxis)
        drawAxis();

    glColor3f(config.group.color.x, config.group.color.y, config.group.color.z);
    glClearColor(config.scene.bgColor.x, config.scene.bgColor.y, config.scene.bgColor.z, config.scene.bgColor.w);
    if (config.scene.useVBOs)
        drawWithVBOs(buffers, verticesCount);
    else
        drawWithoutVBOs(modelsPoints);

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

void updateViewPort(int w, int h)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
    if (h == 0)
        h = 1;

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
            else if (wheelDelta < 0 && config.camera.cameraDistance < 120.0f)
                config.camera.cameraDistance += config.camera.scrollSensitivity;

            config.camera.position.x = config.camera.cameraDistance * cos(config.camera.cameraAngleY) * cos(config.camera.cameraAngle);
            config.camera.position.y = config.camera.cameraDistance * sin(config.camera.cameraAngleY);
            config.camera.position.z = config.camera.cameraDistance * cos(config.camera.cameraAngleY) * sin(config.camera.cameraAngle);
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
                config.camera.isDragging = true;
                config.camera.lastX = x;
                config.camera.lastY = y;
            } else if (state == GLUT_UP) {
                config.camera.isDragging = false;
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
        if (config.camera.isDragging) {
            float dx = (x - config.camera.lastX) * config.camera.sensitivity; // horizontal movement reversed for better feel
            float dy = (y - config.camera.lastY) * config.camera.sensitivity;

            config.camera.cameraAngle += dx;

            // restrict the camera's Y angle to between -PI/2 and PI/2, plus a bit of a tighter squeeze to avoid looking inline with the Y axis
            if ((dy >= 0 && config.camera.cameraAngleY <= M_PI / 2 * 0.95) || (dy < 0 && config.camera.cameraAngleY >= -M_PI / 2 * 0.95)) {
                config.camera.cameraAngleY += dy;
            }

            config.camera.lastX = x;
            config.camera.lastY = y;

            config.camera.position.x = config.camera.cameraDistance * cos(config.camera.cameraAngleY) * cos(config.camera.cameraAngle);
            config.camera.position.y = config.camera.cameraDistance * sin(config.camera.cameraAngleY);
            config.camera.position.z = config.camera.cameraDistance * cos(config.camera.cameraAngleY) * sin(config.camera.cameraAngle);

            glutPostRedisplay();
        }
    }
}

void keyboardFunc(unsigned char key, int x, int y)
{
    if (key == 82 || key == 114) { // r or R
        resetCamera(&config);
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
    glutReshapeFunc(updateViewPort);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboardFunc);
}

void initializeOpenGLContext()
{
#ifndef __APPLE__
    glewInit();
#endif
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(config.scene.bgColor.x, config.scene.bgColor.y, config.scene.bgColor.z, config.scene.bgColor.w);
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

        // also store the points for the non-VBO case
        modelsPoints.push_back(modelPoints);
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
