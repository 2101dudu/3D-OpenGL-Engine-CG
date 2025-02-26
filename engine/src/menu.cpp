#include "menu.hpp"

void initializeImGUI(void)
{
    IMGUI_CHECKVERSION();
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

void initializeMenu(void)
{
    ImGui_ImplGLUT_NewFrame();
    ImGui_ImplOpenGL2_NewFrame();
    ImGui::NewFrame();
}

void drawMenu(void)
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::NewFrame();

    ImGui::Begin("Options");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}
    
void renderDrawData(void) {
    //ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}   

void shutdownMenu(void)
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}