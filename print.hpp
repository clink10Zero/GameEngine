#pragma once

namespace printGUI
{
	class Print {

		public :
			Print()
			{
				//edit
				projectSetting = false;
					settingEditorControle = false;
					settingGameControle = false;
				//windows
				game = true;
				component = true;
				hierarchy = true;
				//file
				save = false;
				open = false;
			};

			//edit
			bool projectSetting;
				bool settingEditorControle;
				bool settingGameControle;
			//windows
			bool game;
			bool component;
			bool hierarchy;
			//file
			bool save;
			bool open;
	};
}