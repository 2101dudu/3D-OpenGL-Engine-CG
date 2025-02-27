#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
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
    ImGui::Text("%.3f ms/frame (%d FPS)", 1000.0f / config->stats.fps, config->stats.fps);
    if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_Framed)) {
        ImGui::SliderFloat("Camera Distance", &config->camera.cameraDistance, 0.5, 120);
        ImGui::DragFloat3("Position", &config->camera.position.x, 0.05f);
        ImGui::DragFloat3("Looking At", &config->camera.lookAt.x, 0.05f);
        ImGui::DragFloat3("Up", &config->camera.up.x, 0.05f);

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Scene options", ImGuiTreeNodeFlags_Framed)) {
        ImGui::Checkbox("Cull faces", &config->scene.faceCulling);
        ImGui::Checkbox("Wireframe", &config->scene.wireframe);

        ImGui::TreePop();
    }
    ImGui::Text("Current triangles: %d", config->stats.numTriangles);

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
