#include "quick-save-reload.h"
#include "pch.h"

#include <fstream>
#include <filesystem>
#include <string>

#define STARTUP_MESSAGE_TIMER_LENGTH 1200
#define RELOAD_MESSAGE_TIMER_LENGTH 270

bool init = false;

bool bad_file = false;
bool bad_path = false;
bool write_failed = false;

bool reload_message_displayed = true;
int reload_message_timer = 0;

int startup_message_timer = 0;

SaveFileData* save_file_data;

std::string save_file_path;
int save_num;

std::string game_save_file_path;
std::string game_save_file_name;

int previous_game_mode;

bool load_save_file_data()
{
	save_file_data = (SaveFileData*)malloc(sizeof(SaveFileData));
	char* buffer = (char*)malloc(sizeof(SaveFileData));
	if (!std::filesystem::exists(save_file_path))
	{
		bad_path = true;
		return false;
	}

	int file_size = std::filesystem::file_size(save_file_path);
	PrintDebug("%d\n", file_size);
	if (file_size != sizeof(SaveFileData))
	{
		bad_file = true;
		return false;
	}
	std::ifstream file(save_file_path, std::ios::in | std::ios::binary);
	if (!file.is_open())
		return false;
	file.read(buffer, sizeof(SaveFileData));
	file.close();

	memcpy(save_file_data, buffer, sizeof(SaveFileData));
	free(buffer);

	return true;
}

bool write_to_dest_save_file()
{
	std::ofstream save_file(game_save_file_path + game_save_file_name, std::ios::out | std::ios::binary);
	if (!save_file.is_open())
		return false;
	save_file.write((char*)save_file_data, sizeof(SaveFileData));
	save_file.close();

	return true;
}

void init_quick_save_reload(std::string savepath, std::string filepath, int savenum)
{
	save_file_path = filepath;
	save_num = savenum;
	game_save_file_path = savepath;

	if (save_file_path.empty())
		return;

	if (save_num < 10)
		game_save_file_name = std::string("SonicDX0" + std::to_string(save_num) + ".snc");
	else
		game_save_file_name = std::string("SonicDX" + std::to_string(save_num) + ".snc");


	PrintDebug("NewFileName: %s\n", game_save_file_name.c_str());

	if (!load_save_file_data())
		return;

	if (!write_to_dest_save_file())
	{
		write_failed = true;
		return;
	}

	init = true;
}

void onFrame_quick_save_reload()
{
	if (previous_game_mode != 12 && GameMode == 12)
	{
		reload_message_displayed = true;
		reload_message_timer = 0;

		memcpy(&SaveFile, save_file_data, sizeof(SaveFileData));
		write_to_dest_save_file();
		LoadSave();
	}

	previous_game_mode = GameMode;

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

			DisplayDebugStringFormatted(NJM_LOCATION(1, 2), "Using \"%s\"", save_file_path.c_str());

			DisplayDebugStringFormatted(NJM_LOCATION(1, 3), "Load the %s save file", game_save_file_name.c_str());

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