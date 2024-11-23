#include "pch.h"
#include "extensions-to-modloader.h"
#include "quick-save-reload.h"
#include "gamma-timer.h"
#include "chao-stat-viewer.h"

#define EXTERNAL_ACCESS_MEMLOC (void*)0x426028
#define EXTERN_MEMSIZE 128
static char* AccessibleMemory;

void FrameLimitSetup(int refreshRate);
void SkipmaFix();

bool isQuickSaveEnabled;

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helper_funcs)
	{
		// Config File Start
		const IniFile* configFile = new IniFile(std::string(path) + "\\config.ini");

		isQuickSaveEnabled = configFile->getBool("QuickSaveSettings", "Enabled", false);
		bool isQuickSaveForced = configFile->getBool("QuickSaveSettings", "ForceLoad", false);
		std::string premadeSave = configFile->getString("QuickSaveSettings", "PremadeFile", "Custom");
		std::string saveFilePath = configFile->getString("QuickSaveSettings", "SaveFilePath");
		int save_num = configFile->getInt("QuickSaveSettings", "SaveNum", 99);
		bool isCCEF_Enabled = configFile->getBool("OtherSettings", "CCEF", true);
		bool isFreeCamStart = configFile->getBool("OtherSettings", "FreeCam", false);
		bool isSkipma_Enabled = configFile->getBool("OtherSettings", "Skipma", true);
		std::string frameLimitOption = configFile->getString("OtherSettings", "FrameLimit", "OFF");

		delete configFile;
		// Config File End

		if (isQuickSaveEnabled)
		{
			std::string chosenFile;
			if (premadeSave == "Custom") {
				chosenFile = saveFilePath;
			}
			else {
				chosenFile = std::string(path) + "\\premadeSaves\\" + premadeSave + ".snc";
			}
			init_quick_save_reload(std::string(helper_funcs.GetMainSavePath()) + "\\", chosenFile, save_num, isQuickSaveForced);
		}

		DisplayChaoStatsInit();

		AccessibleMemory = (char*)malloc(EXTERN_MEMSIZE);
		if (AccessibleMemory == nullptr) {
			PrintDebug("Couldn't allocate memory for external access\n");
		}
		WritePointer(EXTERNAL_ACCESS_MEMLOC, AccessibleMemory);

		AccessibleMemory = init_gamma_timer(AccessibleMemory); // +0x20 bytes


		// FIXES

		// CCEF, rewrite two places where there is a mov instead of or
		if (isCCEF_Enabled)
		{
			WriteData<uint16_t>((uint16_t*)0x434870, 0x0D81);
			WriteData<uint16_t>((uint16_t*)0x438330, 0x0D81);
		}

		if (isFreeCamStart)
			free_camera_mode |= 1;

		if (isSkipma_Enabled) 
			SkipmaFix();

		// Egg Hornet Crash Fix
		WriteNop<3>((void*)0x533939);
		WriteNop<2>((void*)0x440EF5);

		if (frameLimitOption != "OFF")
		{
			int frames = std::stoi(frameLimitOption);
			PrintDebug("Frame limit: %dFPS\n", frames);
			FrameLimitSetup(frames);
		}
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		if (isQuickSaveEnabled) {
			onFrame_quick_save_reload();
		}
		DisplayChaoStatsOnFrame();
		run_gamma_timer();
	}

	__declspec(dllexport) void __cdecl OnInput()
	{

	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}