#include "pch.h"
#include "modules.h"


static bool isQuickSaveEnabled;

static char* accessibleMemory; // For referencing variables externally 

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

		accessibleMemory = (char*) malloc(64);

		PrintDebug("accessMem Pointer Initial: %X\n", accessibleMemory);

		//WritePointer((void*) 0x426028, (int) &accessibleMemory); // Write our memory pointer into padded sonic.exe memory
		WriteData((void*) 0x426028, (void*) &accessibleMemory, 4);

		init_gamma_timer(accessibleMemory); // 0x0C Bytes used in accessibleMemory

		accessibleMemory += 0x20; // Offset by 0x20, might be useful in the future

		init_new_igt(accessibleMemory);

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

			//long double seconds = get_RTA_time() * 0.000001;
			
			//PrintDebug("%Lf\n", seconds);
		}

		get_RTA_time();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}