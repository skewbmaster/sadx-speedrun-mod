#include "pch.h"
#include "quick-save-reload.h"

#include <fstream>
#include <filesystem>
#include <string>

#define STARTUP_MESSAGE_TIMER_LENGTH 600
#define RELOAD_MESSAGE_TIMER_LENGTH 200

bool init = false;
bool force_load = false;
int save_slot_num;

std::string custom_savefile_path;
std::string game_savefile_path;
std::string game_savefile_name;
SaveFileData* savefile_data;

int oldGameMode;
int completedGammaLevels;

bool bad_file = false;
bool bad_path = false;
bool write_failed = false;

bool reload_message_displayed = true;
int reload_message_timer = 0;
int startup_message_timer = 0;

void init_quick_save_reload(std::string savepath, std::string filepath, int savenum, bool forceload)
{
	custom_savefile_path = filepath;
	game_savefile_path = savepath;
	save_slot_num = savenum;
	force_load = forceload;

	if (filepath.empty())
		return;

	if (save_slot_num < 10)
		game_savefile_name = std::string("SonicDX0" + std::to_string(save_slot_num) + ".snc");
	else
		game_savefile_name = std::string("SonicDX" + std::to_string(save_slot_num) + ".snc");

	if (!load_save_file_data())
		return;

	if (!write_to_dest_save_file())
		return;

	init = true;
}

void onFrame_quick_save_reload()
{
	if (init && GameMode == GameModes_Menu) {
		if ((force_load && oldGameMode != GameMode) || ControllerPointers[0]->PressedButtons & Buttons_Y)
		{
			reload_save_file();
		}
	}

	oldGameMode = GameMode;

	ScaleDebugFontQSR(15, true);
	if (startup_message_timer < STARTUP_MESSAGE_TIMER_LENGTH)
	{
		if (!init)
		{
			SetDebugFontColor(0xFFFF0000);
			DisplayDebugString(NJM_LOCATION(1, 1), "SADX Quick Save Reload - WARNING:");

			if (bad_file)
				DisplayDebugString(NJM_LOCATION(1, 2), "Bad save file provided, The mod will not operate");
			else if (bad_path)
				DisplayDebugString(NJM_LOCATION(1, 2), "Provided save file doesn't exist, The mod will not operate");
			else if (write_failed)
				DisplayDebugString(NJM_LOCATION(1, 2), "Could not write the game savedata, The mod will not operate");
			else
				DisplayDebugString(NJM_LOCATION(1, 2), "Save file could not be opened, The mod will not operate");

			int remaining_time = (STARTUP_MESSAGE_TIMER_LENGTH - startup_message_timer) / 60;

			if (startup_message_timer > 150)
			{
				SetDebugFontColor(0xFF00FFAA);
				DisplayDebugStringFormatted(NJM_LOCATION(1, 4), "(This message will disappear in %d seconds)", remaining_time);
			}

		}
		else
		{
			SetDebugFontColor(0xFF00FF00);
			DisplayDebugString(NJM_LOCATION(1, 1), "SADX Quick Save Reload");

			DisplayDebugStringFormatted(NJM_LOCATION(1, 2), "Using \"%s\"", custom_savefile_path.c_str());

			DisplayDebugStringFormatted(NJM_LOCATION(1, 3), "Load the %s save file", game_savefile_name.c_str());

			int remaining_time = (STARTUP_MESSAGE_TIMER_LENGTH - startup_message_timer) / 60;

			if (startup_message_timer > 150)
			{
				SetDebugFontColor(0xFF00FFAA);
				DisplayDebugStringFormatted(NJM_LOCATION(1, 5), "(This message will disappear in %d seconds)", remaining_time);
			}
		}

		reload_message_displayed = false;

		startup_message_timer++;
		return;
	}

	if (reload_message_displayed)
	{
		if (reload_message_timer < RELOAD_MESSAGE_TIMER_LENGTH)
		{
			SetDebugFontColor(0xFF00FFAA);
			DisplayDebugString(NJM_LOCATION(1, 1), "SADX Quick Save Reload");

			DisplayDebugString(NJM_LOCATION(1, 2), "Reloaded save file");

			reload_message_timer++;
		}
		else
			reload_message_displayed = false;
	}
}

void reload_save_file()
{
	int menuState = -1;
	if (SeqTP != nullptr) {
		// The menu task has a variable for the current menu type
		menuState = SeqTP->awp->work.ub[4];
	}

	// Check if we're on the title or gamemode select screen
	if (menuState != 5 && menuState != 6) 
		return;
	

	if ((SaveFile.Emblems[0xB] & 0x8 && !(SaveFile.Emblems[0xB] & 0x2) && completedGammaLevels == 0) // If done with gamma hot shelter but not red mountain, and not incremented
		|| (SaveFile.Emblems[0xB] & 0x2 && completedGammaLevels == 1) // If done with red mountain and only incremented levels once
		|| (SaveFile.Emblems[0xB] & 0x1 && completedGammaLevels == 2)) // If done with windy valley and only incremented levels twice
	{
		if (force_load && oldGameMode != GameMode) {
			completedGammaLevels++;
			return;
		}
	}

	completedGammaLevels = 0;

	reload_message_displayed = true;
	reload_message_timer = 0;

	write_to_dest_save_file(); // Write initial save data into file immediately

	// This list gets cleared after you choose a file, we should do the same
	GCMemoca_State.u8FileName = save_slot_num;
	if (GCMemoca_State.string == nullptr)
	{
		GCMemoca_State.string = (char*) malloc(14); // SonicDX00.snc
		if (GCMemoca_State.string == nullptr)
		{
			return;
		}
	}
	strcpy_s(GCMemoca_State.string, 14, game_savefile_name.c_str());

	memcpy_s(&SaveFile, sizeof(SaveFileData), savefile_data, sizeof(SaveFileData)); // Load save data into what the game will use for save loading
	LoadSave();
}

bool load_save_file_data()
{
	savefile_data = (SaveFileData*) malloc(sizeof(SaveFileData));
	if (savefile_data == nullptr) // Allocate memory for custom file data
		return false;

	if (!std::filesystem::exists(custom_savefile_path)) // Check if custom file exists
	{
		bad_path = true;
		return false;
	}

	int file_size = std::filesystem::file_size(custom_savefile_path);
	if (file_size != sizeof(SaveFileData)) // Check if file size is correct
	{
		bad_file = true;
		return false;
	}
	std::ifstream file(custom_savefile_path, std::ios::in | std::ios::binary);
	if (!file.is_open())
		return false;
	file.read((char*)savefile_data, sizeof(SaveFileData)); // Open and read custom file, save data to persistent memory
	file.close();

	return true;
}


bool write_to_dest_save_file()
{
	std::ofstream save_file(game_savefile_path + game_savefile_name, std::ios::out | std::ios::binary);
	if (!save_file.is_open()) // Create/overwrite the SonicDX.snc file at the desired slot
	{
		write_failed = true;
		return false;
	}
	save_file.write((char*)savefile_data, sizeof(SaveFileData));
	save_file.close();

	return true;
}


void ScaleDebugFontQSR(int scale, bool enableFontScaling)//thanks to PkR for this code https://github.com/PiKeyAr/sadx-debug-mode/
{
	float FontScale;
	if (!enableFontScaling) FontScale = 1.0f;
	else
	{
		if ((float)HorizontalResolution / (float)VerticalResolution > 1.33f) FontScale = floor((float)VerticalResolution / 480.0f);
		else FontScale = floor((float)HorizontalResolution / 640.0f);
	}
	SetDebugFontSize(FontScale * scale);
}