#include "structs.hpp"
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
#include <OpenGL/gl.h>
#elif _WIN32
#include <GL/glew.h>
// DONT REMOVE THIS COMMENT
#include <GL/freeglut.h>
#include <Windows.h>
extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#else
#include <GL/glew.h>
// DONT REMOVE THIS COMMENT
#include <GL/freeglut.h>
#include <GL/glut.h>
#endif

int lastRealTime;
float globalTimer = 0.0f;

GLfloat g_viewMatrix[16];

// the time update factor
float timeFactor = 1;

static bool ignoreWarp = false;
static const int warpThreshold = 50;
static float smoothedAngle = 0.0f;
static float smoothedAngleY = 0.0f;

WorldConfig config;

bool drawCatmullRomCurves = false;

// VBOs
std::vector<GLuint> buffers;

void updateSceneOptions(void)
{
    uint32_t mode = config.scene.wireframe ? GL_LINE : GL_FILL;

    glPolygonMode(GL_FRONT_AND_BACK, mode);

    if (config.scene.faceCulling) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    drawCatmullRomCurves = config.scene.drawCatmullRomCurves;
}

void renderScene(void)
{
    // update global timers
    int currentRealTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaRealTime = (currentRealTime - lastRealTime);
    float deltaTime = deltaRealTime * timeFactor;
    globalTimer += deltaTime;
    lastRealTime = currentRealTime;

    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    updateCameraLookAt(&config);
    gluLookAt(config.camera.position.x, config.camera.position.y, config.camera.position.z,
        config.camera.lookAt.x, config.camera.lookAt.y, config.camera.lookAt.z,
        config.camera.up.x, config.camera.up.y, config.camera.up.z);

    // capture the view matrix (camera transformation)
    glGetFloatv(GL_MODELVIEW_MATRIX, g_viewMatrix);

    if (config.scene.drawAxis)
        drawAxis();

    glClearColor(config.scene.bgColor.x, config.scene.bgColor.y, config.scene.bgColor.z, config.scene.bgColor.w);
    drawWithVBOs(buffers, config.group, false);

    drawMenu(&config);

    // update scene options based on the menu
    updateSceneOptions();

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

unsigned char picking(int x, int y)
{
    // TODO: Unecessary for now
    // glDisable(GL_LIGHTING);
    // glDisable(GL_TEXTURE_2D);

    // set background color to black to differenciate values > 0
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glDepthFunc(GL_LEQUAL);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(config.camera.position.x, config.camera.position.y, config.camera.position.z,
        config.camera.lookAt.x, config.camera.lookAt.y, config.camera.lookAt.z,
        config.camera.up.x, config.camera.up.y, config.camera.up.z);

    // re-render scene
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    drawWithVBOs(buffers, config.group, true);

    unsigned char res[4];
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);
    glReadPixels(x, viewport[3] - y,
        1, 1,
        GL_RGBA, GL_UNSIGNED_BYTE,
        res);

    // TODO: Unecessary for now
    // glEnable(GL_LIGHTING);
    // glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LESS);

    return res[0];
}

void focusGroup(unsigned char picked)
{
    GroupConfig* g = config.clickableGroups[picked];
    config.camera.tracking = g == NULL ? 0 : picked;
    config.camera.showInfoWindow = true;
}

// track mouse scroll events
void mouseWheel(int wheel, int direction, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    float wheelDelta = (direction > 0) ? -1.0f : 1.0f;
    io.AddMouseWheelEvent(0.0f, wheelDelta);

    if (!io.WantCaptureMouse) {
        if (config.camera.isOrbital) {

            float distance = config.camera.cameraDistance;
            float adaptiveSpeed = config.camera.scrollSensitivity * std::max(0.01f, distance * 0.4f);

            config.camera.cameraDistance += wheelDelta * adaptiveSpeed;
            if (config.camera.cameraDistance < config.camera.projection.near1)
                config.camera.cameraDistance = config.camera.projection.near1;
            else if (config.camera.cameraDistance > config.camera.projection.far1)
                config.camera.cameraDistance = config.camera.projection.far1;

            updateCamera(&config);
            glutPostRedisplay();
        }
    }
}

// track mouse button presses
void mouseButton(int button, int state, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();

    // this assert stops imgui from trying to add a mouse button event of a scroll that would result in a segmentation fault
    if (button >= 0 && button < ImGuiMouseButton_COUNT) {
        io.AddMouseButtonEvent(button, state == GLUT_DOWN);
    }

    // click and drag
    if (!io.WantCaptureMouse) {
        if (config.camera.isOrbital && button == GLUT_LEFT_BUTTON) {
            config.camera.isDragging = (state == GLUT_DOWN);
            if (config.camera.isDragging) {
                config.camera.lastX = x;
                config.camera.lastY = y;
            }
        }

        if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
            unsigned char picked = picking(x, y);
            focusGroup(picked);
            // glutPostRedisplay();
        }
    }
}

// track mouse movement
void mouseMotion(int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);

    if (!io.WantCaptureMouse) {
        if (config.camera.isDragging && config.camera.isOrbital) { // orbital mode
            float dx = (x - config.camera.lastX) * config.camera.sensitivity; // horizontal movement reversed for better feel
            float dy = (y - config.camera.lastY) * config.camera.sensitivity;

            config.camera.cameraAngle += dx;

            // restrict the camera's Y angle to between -PI/2 and PI/2, plus a bit of a tighter squeeze to avoid looking inline with the Y axis
            if ((dy >= 0 && config.camera.cameraAngleY <= M_PI / 2 * 0.95) || (dy < 0 && config.camera.cameraAngleY >= -M_PI / 2 * 0.95)) {
                config.camera.cameraAngleY += dy;
            }

            config.camera.lastX = x;
            config.camera.lastY = y;

            updateCamera(&config);
            glutPostRedisplay();
        } else if (!config.camera.isOrbital) { // FPS mode
            if (ignoreWarp) {
                ignoreWarp = false;
                return;
            }

            int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
            int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

            float dx = (x - centerX) * config.camera.sensitivity;
            float dy = (centerY - y) * config.camera.sensitivity; // Inverted Y-axis for correct movement

            float alpha = 0.2f; // smoothing factor (0.1 - 0.3 works well)
            smoothedAngle = smoothedAngle * (1 - alpha) + dx * alpha;
            smoothedAngleY = smoothedAngleY * (1 - alpha) + dy * alpha;

            config.camera.cameraAngle += smoothedAngle;
            config.camera.cameraAngleY += smoothedAngleY;

            // Clamp vertical angle to prevent flipping
            float limit = M_PI / 2 * 0.95;
            if (config.camera.cameraAngleY > limit)
                config.camera.cameraAngleY = limit;
            if (config.camera.cameraAngleY < -limit)
                config.camera.cameraAngleY = -limit;

            // Update camera look direction
            float cosY = cosf(config.camera.cameraAngleY);
            float dirX = cosf(config.camera.cameraAngle) * cosY;
            float dirY = sinf(config.camera.cameraAngleY);
            float dirZ = sinf(config.camera.cameraAngle) * cosY;

            config.camera.lookAt.x = config.camera.position.x + dirX;
            config.camera.lookAt.y = config.camera.position.y + dirY;
            config.camera.lookAt.z = config.camera.position.z + dirZ;

            // Reset mouse position to center
            ignoreWarp = true;
            glutWarpPointer(centerX, centerY);

            glutPostRedisplay();
        }
    }
}

void keyboardFunc(unsigned char key, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGLUT_KeyboardFunc(key, x, y);

    if (!io.WantCaptureKeyboard) {
        if (key == 82 || key == 114) { // R or r
            resetCamera(&config);
        }
        if (key == 27) { // ESC
            switchCameraMode(&config);

            glutWarpPointer(config.window.width / 2, config.window.height / 2);

            // hide cursor on FPS
            glutSetCursor(config.camera.isOrbital ? GLUT_CURSOR_INHERIT : GLUT_CURSOR_CROSSHAIR);
        }
        if (!config.camera.isOrbital) {
            // Compute Forward Vector
            float forwardX = config.camera.lookAt.x - config.camera.position.x;
            float forwardY = config.camera.lookAt.y - config.camera.position.y;
            float forwardZ = config.camera.lookAt.z - config.camera.position.z;

            // Normalize Forward Vector
            float forwardLength = sqrt(forwardX * forwardX + forwardY * forwardY + forwardZ * forwardZ);
            if (forwardLength != 0) {
                forwardX /= forwardLength;
                forwardY /= forwardLength;
                forwardZ /= forwardLength;
            }

            // Define World Up Vector
            float upX = 0.0f, upY = 1.0f, upZ = 0.0f;

            // Compute Right Vector (Cross Product of Forward and Up)
            float rightX = (forwardY * upZ - forwardZ * upY);
            float rightY = (forwardZ * upX - forwardX * upZ);
            float rightZ = (forwardX * upY - forwardY * upX);

            // Normalize Right Vector
            float rightLength = sqrt(rightX * rightX + rightY * rightY + rightZ * rightZ);
            if (rightLength != 0) {
                rightX /= rightLength;
                rightY /= rightLength;
                rightZ /= rightLength;
            }

            // Movement Speed
            float speed = 1.0f;

            // Movement Input
            if (key == 87 || key == 119) { // W or w (Move Forward)
                config.camera.position.x += forwardX * speed;
                config.camera.position.y += forwardY * speed;
                config.camera.position.z += forwardZ * speed;
            }
            if (key == 83 || key == 115) { // S or s (Move Backward)
                config.camera.position.x -= forwardX * speed;
                config.camera.position.y -= forwardY * speed;
                config.camera.position.z -= forwardZ * speed;
            }
            if (key == 65 || key == 97) { // A or a (Move Left)
                config.camera.position.x -= rightX * speed;
                config.camera.position.y -= rightY * speed;
                config.camera.position.z -= rightZ * speed;
            }
            if (key == 68 || key == 100) { // D or d (Move Right)
                config.camera.position.x += rightX * speed;
                config.camera.position.y += rightY * speed;
                config.camera.position.z += rightZ * speed;
            }

            config.camera.lookAt.x = config.camera.position.x + forwardX;
            config.camera.lookAt.y = config.camera.position.y + forwardY;
            config.camera.lookAt.z = config.camera.position.z + forwardZ;

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

void createWindowWithConfig()
{
    glutInitWindowSize(config.window.width, config.window.height);
    glutCreateWindow("CG@DI");
}

void setupCallbacks()
{
    glutIdleFunc(renderScene);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(updateViewPort);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboardFunc);
    glutMouseWheelFunc(mouseWheel); // Register the mouse wheel callback
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

void bindPointsToBuffers()
{
    std::map<std::string, Model*>::iterator it;
    int count = 0;
    for (it = config.filesModels.begin(); it != config.filesModels.end(); it++, count++) {
        Model* model = it->second;

        ModelInfo modelInfo = parseFile(model->file);
        std::vector<float> modelPoints = modelInfo.points;

        model->vboIndex = count;
        model->vertexCount = modelPoints.size() / 3;
        model->triangleCount = modelInfo.numTriangles;

        glBindBuffer(GL_ARRAY_BUFFER, buffers[count]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * modelPoints.size(), modelPoints.data(), GL_STATIC_DRAW);
    }
}

void pointModelsVBOIndex(GroupConfig* group)
{
    for (auto& model : group->models) {
        Model* m = config.filesModels[model->file];
        model = m;
        config.stats.numTriangles += model->triangleCount;
    }

    for (auto& subGroup : group->children) {
        pointModelsVBOIndex(subGroup);
    }
}

void initializeVBOs()
{
    glEnableClientState(GL_VERTEX_ARRAY);

    // count the number of models
    int totalNumModels = config.filesModels.size();

    // resize VBOs' buffers
    buffers.resize(totalNumModels);
    glGenBuffers(totalNumModels, buffers.data());

    // bind all of the files' models to the VBOs' buffer
    bindPointsToBuffers();

    // recursively make the models point to their respective VBO index
    pointModelsVBOIndex(&config.group);

    // TODO: check if a reset on the buffer's index is needed
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
    createWindowWithConfig();

    // initialize the OpenGL context
    initializeOpenGLContext();

    // initialize VBOs
    initializeVBOs();

    // initialize menu
    initializeImGUI();

    // setup callbacks
    setupCallbacks();

    // enter the GLUT main loop
    lastRealTime = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();

    shutdownMenu();

    return 1;
}
