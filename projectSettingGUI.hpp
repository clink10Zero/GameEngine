#pragma once
#include "projectSetting.hpp"

namespace lve {
	class GUIProjectSetting
	{
	public:
		GUIProjectSetting() = default;
		~GUIProjectSetting() = default;

		void OnImGuiRender();

		void SettingEditorControle();

		void SettingGameControle();
	};
}