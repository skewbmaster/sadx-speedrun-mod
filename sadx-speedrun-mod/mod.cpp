#include "pch.h"
#include "quick-save-reload.h"


extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helper_funcs)
	{
		// Config File Start
		const IniFile* config_file = new IniFile(std::string(path) + "\\config.ini");

		std::string save_file_path = config_file->getString("QuickSaveSettings", "SaveFilePath");
		int save_num = config_file->getInt("QuickSaveSettings", "SaveNum", 99);

		delete config_file;
		// Config File End 

		PrintDebug("Savenum: %d\n", save_num);

		init_quick_save_reload(std::string(helper_funcs.GetMainSavePath()) + "\\", save_file_path, save_num);



	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		onFrame_quick_save_reload();
	}


	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}