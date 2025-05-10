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
std::vector<GLuint> vboBuffers;
std::vector<GLuint> vboBuffersNormals;
std::vector<GLuint> iboBuffers; // armazena IBOs de índices

const char* configFilePath;
bool hotReload = false;

const float dark[] = { 0.2f, 0.2f, 0.2f, 1.0f };
const float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };

WorldConfig loadConfiguration(const char* configFile)
{
    WorldConfig cfg = XMLParser::parseXML(configFile);
    XMLParser::configureFromXML(cfg);
    return cfg;
}

void bindPointsToBuffers()
{
    int count = 0;
    for (auto it = config.filesModels.begin(); it != config.filesModels.end(); ++it, ++count) {
        const std::string& fname = it->first;
        Model* model = it->second;

        ModelInfo mi = parseFile(model->file);

        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, vboBuffers[count]);
        glBufferData(GL_ARRAY_BUFFER,
            mi.points.size() * sizeof(float),
            mi.points.data(),
            GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vboBuffersNormals[count]);
        glBufferData(GL_ARRAY_BUFFER,
            mi.normals.size() * sizeof(float),
            mi.normals.data(),
            GL_STATIC_DRAW);

        // IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboBuffers[count]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            mi.indices.size() * sizeof(unsigned int),
            mi.indices.data(),
            GL_STATIC_DRAW);

        // Stores in Model
        model->vboIndex = count;
        model->iboIndex = count;
        model->vertexCount = mi.points.size() / 3;
        model->indexCount = mi.indices.size();
        model->triangleCount = mi.numTriangles;
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

    int totalNumModels = config.filesModels.size();
    vboBuffers.resize(totalNumModels);
    vboBuffersNormals.resize(totalNumModels);
    iboBuffers.resize(totalNumModels);
    glGenBuffers(totalNumModels, vboBuffers.data());
    glGenBuffers(totalNumModels, vboBuffersNormals.data());
    glGenBuffers(totalNumModels, iboBuffers.data());

    bindPointsToBuffers();

    // Turns off buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    pointModelsVBOIndex(&config.group);
}

void updateSceneOptions(void)
{
    uint32_t mode = config.scene.wireframe ? GL_LINE : GL_FILL;

    glPolygonMode(GL_FRONT_AND_BACK, mode);

    if (config.scene.faceCulling) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    if (config.scene.lighting) {
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }

    if (hotReload) {
        config = loadConfiguration(configFilePath);
        initializeVBOs();
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

    if (config.scene.drawAxis) {
        if (config.scene.lighting)
            glDisable(GL_LIGHTING);
        drawAxis();
        if (config.scene.lighting)
            glEnable(GL_LIGHTING);
    }

    for (size_t i = 0; i < config.lights.size(); ++i) {
        const LightConfig& light = config.lights[i];
        GLenum lightID = GL_LIGHT0 + static_cast<GLenum>(i);
        glEnable(lightID);

        glLightfv(lightID, GL_AMBIENT, dark);
        glLightfv(lightID, GL_DIFFUSE, white);
        glLightfv(lightID, GL_SPECULAR, white);
        glLightfv(lightID, GL_POSITION, light.position);

        if (light.type == LightType::SPOTLIGHT) {
            glLightfv(lightID, GL_SPOT_DIRECTION, light.direction);
            glLightf(lightID, GL_SPOT_CUTOFF, light.cutoff);
        } else {
            // For point or directional, use default OpenGL cutoff
            glLightf(lightID, GL_SPOT_CUTOFF, 180.0f);
        }
    }

    glClearColor(config.scene.bgColor.x, config.scene.bgColor.y, config.scene.bgColor.z, config.scene.bgColor.w);

    // glutSolidSphere(1, 10, 10);
    drawWithVBOs(vboBuffers, vboBuffersNormals, iboBuffers, config.group, false);

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
    if (config.scene.lighting)
        glDisable(GL_LIGHTING);
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
    drawWithVBOs(vboBuffers, vboBuffersNormals, iboBuffers, config.group, true);

    unsigned char res[4];
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);
    glReadPixels(x, viewport[3] - y,
        1, 1,
        GL_RGBA, GL_UNSIGNED_BYTE,
        res);

    if (config.scene.lighting)
        glEnable(GL_LIGHTING);
    // glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LESS);

    return res[0];
}

void focusGroup(unsigned char picked)
{
    GroupConfig* g = config.clickableGroups[picked];
    config.camera.tracking = g == NULL ? 0 : picked;
    config.camera.showInfoWindow = g == NULL ? false : true;

    if (g != NULL) {
        config.camera.isOrbital = true;

        // hide cursor on FPS
        glutSetCursor(config.camera.isOrbital ? GLUT_CURSOR_INHERIT : GLUT_CURSOR_CROSSHAIR);
    }
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

            if (config.camera.tracking != 0) {
                updateCamera(&config);
            }
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
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_RESCALE_NORMAL);

    float black[4] = { 0.1f, 0.1f, 0.1f, 0.0f };
    // controls global ambient light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config.xml>" << std::endl;
        return 1;
    }

    // pre-window initialization: Setup GLUT and load configuration
    initializeGLUTPreWindow(argc, argv);
    configFilePath = argv[1];
    config = loadConfiguration(configFilePath);

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
