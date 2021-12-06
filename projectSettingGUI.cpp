#include "projectSettingGUI.hpp"

#include "projectSetting.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "print.hpp"

using namespace printGUI;

using namespace setting;

extern Print affichage;

extern ProjectSetting pSetting;

namespace lve
{
	void GUIProjectSetting::OnImGuiRender()
	{
		ImGui::Begin("Project setting");

		ImGui::Columns(2);

		if (ImGui::BeginMenu("Setting", true))
		{
			if (ImGui::MenuItem("Editor controle", NULL, &affichage.settingEditorControle))
			{
				affichage.settingGameControle = false;
			}
			if (ImGui::MenuItem("Game controle", NULL, &affichage.settingGameControle))
			{
				affichage.settingEditorControle = false;
			}
			ImGui::End();
		}

		ImGui::NextColumn();
		
		if (affichage.settingEditorControle)
		{

		}
		else if (affichage.settingGameControle) {

		}
		ImGui::NewLine();
		
		if (ImGui::Button("valide"))
		{
			affichage.projectSetting = false;
			affichage.settingEditorControle = false;
			affichage.settingGameControle = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("cancel"))
		{
			affichage.projectSetting = false;
			affichage.settingEditorControle = false;
			affichage.settingGameControle = false;
		}
		
		ImGui::End();
	}

	void GUIProjectSetting::SettingEditorControle()
	{
		ImGui::Text("Editor Controle");
		ImGui::SameLine();
		if (ImGui::SmallButton("+"))
		{
			std::vector<Key> keys = pSetting.getKeys();
			char* stringKey[200];

			for (int i = 0; i < keys.size(); i++)
			{
				//stringKey[i] = std::to_string(keys[0]).c_str();
			}
		}
		ImGui::Separator();
	}

	void GUIProjectSetting::SettingGameControle()
	{

	}

}