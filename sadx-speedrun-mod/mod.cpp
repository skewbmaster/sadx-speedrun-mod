#include "pch.h"
#include "modules.h"

static bool isQuickSaveEnabled;
static bool isFreeCamEnabled;

static char* accessibleMemory; // For referencing variables externally 

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helper_funcs)
	{
		// Config File Start
		const IniFile* configFile = new IniFile(std::string(path) + "\\config.ini");

		isQuickSaveEnabled = configFile->getBool("QuickSaveSettings", "Enabled", false);
		isFreeCamEnabled = configFile->getBool("OtherSettings", "FreeCam", false);
		std::string premadeSave = configFile->getString("QuickSaveSettings", "PremadeFile", "Custom");
		std::string saveFilePath = configFile->getString("QuickSaveSettings", "SaveFilePath");
		int save_num = configFile->getInt("QuickSaveSettings", "SaveNum", 99);
		bool isCCEF_Enabled = configFile->getBool("OtherSettings", "CCEF", true);
		
		delete configFile;
		// Config File End

		accessibleMemory = (char*) malloc(64);
		PrintDebug("accessMem Pointer Initial: %X\n", accessibleMemory);

		WritePointer((void*) 0x426028, reinterpret_cast<int>(accessibleMemory)); // Write our memory pointer into padded sonic.exe memory

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
		
		displayChaoStatsInit();

		// CCEF, rewrite two places where there is a mov instead of or
		if (isCCEF_Enabled)
		{
			WriteData<uint16_t>((uint16_t*) 0x434870, 0x0D81);
			WriteData<uint16_t>((uint16_t*) 0x438330, 0x0D81);
		}

		// Egg Hornet Crash Fix
		WriteNop<3>((void*) 0x533939);
		WriteNop<2>((void*) 0x440EF5);

		if (isFreeCamEnabled)
			WriteData<uint8_t>((uint8_t*) 0x03B2CBA8, 7);
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		if (isQuickSaveEnabled)
		{
			onFrame_quick_save_reload();
		}
		DisplayChaoStatsOnFrame();
	}

	__declspec(dllexport) void __cdecl OnInput()
	{
		run_gamma_timer();

		get_RTA_time();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}
