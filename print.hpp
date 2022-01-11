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
				playPause = true;
				game = true;
				component = true;
				hierarchy = true;
				//file
				save = false;
				open = false;
				//windows for component
				dataChunk = false;
					height = true;
					block = false;
			};

			//edit
			bool projectSetting;
				bool settingEditorControle;
				bool settingGameControle;
			//windows
			bool playPause;
			bool game;
			bool component;
			bool hierarchy;

			//file
			bool save;
			bool open;

			//windows for component
			bool dataChunk;
				bool height;
				bool block;
	};
}