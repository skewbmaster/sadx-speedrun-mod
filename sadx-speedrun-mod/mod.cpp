#include "pch.h"
#include "quick-save-reload.h"


extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helper_funcs)
	{
		// Config File Start
		const IniFile* configFile = new IniFile(std::string(path) + "\\config.ini");

		std::string premadeSave = configFile->getString("QuickSaveSettings", "PremadeFile", "Custom");
		std::string saveFilePath = configFile->getString("QuickSaveSettings", "SaveFilePath");
		int save_num = configFile->getInt("QuickSaveSettings", "SaveNum", 99);

		delete configFile;
		// Config File End

		PrintDebug("Savenum: %d\n", save_num);

		if (premadeSave == "Custom")
		{
			init_quick_save_reload(std::string(helper_funcs.GetMainSavePath()) + "\\", saveFilePath, save_num);
		}
		else
		{
			init_quick_save_reload(std::string(helper_funcs.GetMainSavePath()) + "\\", std::string(path) + "\\premadeSaves\\" + premadeSave + ".snc", save_num);
		}

		

	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		onFrame_quick_save_reload();
	}


	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}