#include "structs.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#endif

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include "menu.hpp"
#include "xml_parser.hpp"

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

    ImGui::Begin("Settings");
    if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_Framed)) {
        if (ImGui::SliderFloat("Camera Distance", &config->camera.cameraDistance, 0.5, 120)) {
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
        ImGui::Checkbox("Use VBOs", &config->scene.useVBOs);
        ImGui::ColorEdit3("Background color", (float*)&config->scene.bgColor);

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Models", ImGuiTreeNodeFlags_Framed)) {
        ImGui::ColorEdit3("Model color", (float*)&config->group.color);

        ImGui::TreePop();
    }
    ImGui::Text("Stats:");
    ImGui::Text(">> %.3f ms/frame (%d FPS)", 1000.0f / config->stats.fps, config->stats.fps);
    ImGui::Text(">> Current triangles: %lld", config->stats.numTriangles);

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
