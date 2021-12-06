#pragma once

#include "projectSettingGUI.hpp"

using namespace setting;

namespace lve {
	class Menu {
		public :
			Menu() = default;
			void OnImGuiRender();
		private:
			GUIProjectSetting guiSetting{};
	};
}