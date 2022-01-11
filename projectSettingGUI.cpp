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
		ImGui::SetColumnWidth(0, 200.f);
		ImGui::SetColumnWidth(1, 300.f);
		
		if (ImGui::Button("Editor controle"))
		{
			affichage.settingEditorControle = true;
			affichage.settingGameControle = false;
		}
		if (ImGui::Button("Game controle"))
		{
			affichage.settingEditorControle = false;
			affichage.settingGameControle = true;
		}

		ImGui::NextColumn();

		if (affichage.settingEditorControle)
		{
			SettingEditorControle();
		}
		else if (affichage.settingGameControle)
		{
			SettingGameControle();
		}
		
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

	}

	void GUIProjectSetting::SettingGameControle()
	{
		ImGui::Text("Game Controle");
		ImGui::SameLine();
		static bool add = false;
		if (ImGui::SmallButton("+"))
		{
			add = true;
		}
		if (add)
		{

			ImGui::Text("Name : ");
			ImGui::SameLine();
			char file[250] = "";
			ImGui::InputText("", file, 250);
			ImGui::SameLine();

			if (ImGui::SmallButton("V"))
			{
				//pSetting.getGameControle().addElement(std::string(file), keys[selected]);
				add = false;
			}
		}

		ImGui::Separator();

		std::map<std::string, Key> mapG = pSetting.getGameControle().getData();
		std::map<std::string, Key>::iterator iter;

		for (iter = mapG.begin(); iter != mapG.end(); iter++)
		{
			ImGui::Text((iter->first).c_str());
			ImGui::SameLine();
			ImGui::Text(std::to_string(iter->second).c_str());
			ImGui::SameLine();
			if (ImGui::SmallButton("X"))
			{
				pSetting.getGameControle().remove(iter->first);
			}
		}
	}
}