#include "pch.h"
#include "modules.h"

static bool isQuickSaveEnabled;

static char* injectedMemory;

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helper_funcs)
	{
		// Config File Start
		const IniFile* configFile = new IniFile(std::string(path) + "\\config.ini");

		isQuickSaveEnabled = configFile->getBool("QuickSaveSettings", "Enabled", false);
		std::string premadeSave = configFile->getString("QuickSaveSettings", "PremadeFile", "Custom");
		std::string saveFilePath = configFile->getString("QuickSaveSettings", "SaveFilePath");
		int save_num = configFile->getInt("QuickSaveSettings", "SaveNum", 99);
		

		delete configFile;
		// Config File End

		PrintDebug("Savenum: %d\n", save_num);

		if (isQuickSaveEnabled)
		{
			if (premadeSave == "Custom")
			{
				init_quick_save_reload(std::string(helper_funcs.GetMainSavePath()) + "\\", saveFilePath, save_num);
			}
			else
			{
				init_quick_save_reload(std::string(helper_funcs.GetMainSavePath()) + "\\", std::string(path) + "\\premadeSaves\\" + premadeSave + ".snc", save_num);
			}
		}

		injectedMemory = (char*)malloc(40);

		init_gamma_timer(injectedMemory); // 17 Bytes used in injected mem
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		if (isQuickSaveEnabled)
		{
			onFrame_quick_save_reload();
		}
	}

	__declspec(dllexport) void __cdecl OnInput()
	{
		if (FrameCounter > 120)
		{
			run_gamma_timer();
		}
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}