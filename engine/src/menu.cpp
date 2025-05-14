#include "structs.hpp"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#endif

#include "draw.hpp"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include "menu.hpp"
#include "xml_parser.hpp"

extern float timeFactor;
extern bool hotReload;
extern bool screenshot;

void initializeImGUI(void)
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::GetStyle().WindowRounding = 3.0f;
    ImGui::StyleColorsDark();

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL2_Init();
    ImGui_ImplGLUT_InstallFuncs();
}

void drawMenu(WorldConfig* config)
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin("Settings");
    if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_Framed)) {
        if (ImGui::SliderFloat("Camera Distance", &config->camera.cameraDistance, config->camera.projection.near1, config->camera.projection.far1)) {
            updateCamera(config);
            glutPostRedisplay();
        }
        ImGui::DragFloat3("Position", &config->camera.position.x, 0.05);
        ImGui::DragFloat3("Looking At", &config->camera.lookAt.x, 0.05);
        ImGui::DragFloat3("Up", &config->camera.up.x, 0.05);

        float displayValueMouse = 10.0f + (config->camera.sensitivity - 0.0005f) / (0.01f - 0.0005f) * (100.0f - 10.0f);
        if (ImGui::SliderFloat("Camera Sensitivity", &displayValueMouse, 10.0f, 100.0f)) {
            config->camera.sensitivity = 0.0005f + (displayValueMouse - 10.0f) / (100.0f - 10.0f) * (0.01f - 0.0005f);
        }

        float displayValueZoom = 10.0f + (config->camera.scrollSensitivity - 0.01f) / (0.1f - 0.01f) * (80.0f - 10.0f);
        if (ImGui::SliderFloat("Zoom Sensitivity", &displayValueZoom, 10.0f, 80.0f)) {
            config->camera.scrollSensitivity = 0.01f + (displayValueZoom - 10.0f) / (80.0f - 10.0f) * (0.1f - 0.01f);
        }

        if (ImGui::Button("(R)eset camera")) {
            resetCamera(config);
            glutPostRedisplay();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_Framed)) {
        ImGui::Checkbox("Cull faces", &config->scene.faceCulling);
        ImGui::Checkbox("Wireframe", &config->scene.wireframe);
        ImGui::Checkbox("Draw axis", &config->scene.drawAxis);
        ImGui::Checkbox("Draw Catmull-Rom curves", &config->scene.drawCatmullRomCurves);
        ImGui::Checkbox("Lighting", &config->scene.lighting);
        ImGui::Checkbox("Textures", &config->scene.textures);
        ImGui::ColorEdit3("Background color", (float*)&config->scene.bgColor);

        if (ImGui::SliderFloat("Time scale", &timeFactor, 0.0, 10)) { }

        hotReload = false;
        if (ImGui::Button("Reload config file")) {
            hotReload = true;
        }

        screenshot = false;
        if (ImGui::Button("Take screenshot")) {
            screenshot = true;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Models", ImGuiTreeNodeFlags_Framed)) {
        ImGui::ColorEdit3("Model color", (float*)&config->group.color);

        ImGui::TreePop();
    }
    ImGui::Text("Stats:");
    ImGui::Text(">> %.0f FPS", io.Framerate);
    ImGui::Text(">> Current triangles: %ld", config->stats.numTriangles);

    // render group info
    const unsigned char tracking = config->camera.tracking;
    if (config->camera.showInfoWindow) {
        GroupConfig* g = config->clickableGroups[tracking];

        ImVec2 windowSize(300, 400);
        ImVec2 windowPos(ImGui::GetIO().DisplaySize.x - windowSize.x - 10, 10);

        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);

        ImGui::Begin("Group Info", &config->camera.showInfoWindow, ImGuiWindowFlags_NoResize);
        ImGui::Text("%s:", g->name.c_str());
        ImGui::TextWrapped("%s", g->infoText.c_str());
        ImGui::End();
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void shutdownMenu(void)
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}
