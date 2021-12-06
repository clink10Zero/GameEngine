#include "menu.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "print.hpp"

#include "ecs/coordinator.hpp"

#include <string>

using namespace printGUI;

extern Coordinator gCoordinator;

extern Print affichage;

namespace lve
{

    void  Menu::OnImGuiRender()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File", true))
            {
                ImGui::MenuItem("Open", "Ctrl+O");
                ImGui::MenuItem("Save", "Ctrl+S", &affichage.save);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit", true))
            {
                ImGui::MenuItem("Project setting", "Ctrl+P", &affichage.projectSetting);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Windows", true))
            {
                if (ImGui::BeginMenu("Basique", true))
                {
                    ImGui::MenuItem("Game", NULL, &affichage.game);
                    ImGui::MenuItem("Hierarchy", NULL, &affichage.hierarchy);
                    ImGui::MenuItem("Component", NULL, &affichage.component);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (affichage.save)
        {

            ImGui::Begin("Save");
            char file[250] = "";
            string path = "scene/";
            ImGui::InputText("name", file, 250);

            if (ImGui::Button("Save"))
            {
                gCoordinator.SaveToFile(path + std::string(file) + ".scene", 0);
                affichage.save = false;
            }
            ImGui::End();
        }

        if (affichage.projectSetting)
        {
            guiSetting.OnImGuiRender();
        }
	}
}
