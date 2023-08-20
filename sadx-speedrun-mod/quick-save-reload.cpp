#include "pch.h"
#include "modules.h"

#include <fstream>
#include <filesystem>
#include <string>

#define STARTUP_MESSAGE_TIMER_LENGTH 900
#define RELOAD_MESSAGE_TIMER_LENGTH 200

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

bool is_force_reload_enabled;
bool was_y_pressed;

std::string game_save_file_path;
std::string game_save_file_name;

int oldGameMode;
uint8_t oldMusic;
int completedGammaLevels = 0;

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

bool load_save_file_data();
bool write_to_dest_save_file();
void inject_failsafe_code();

void init_quick_save_reload(std::string savepath, std::string filepath, int savenum, bool isForceReloadEnabled)
{
	save_file_path = filepath;
	game_save_file_path = savepath;

	save_num = savenum;

	is_force_reload_enabled = isForceReloadEnabled;

	inject_failsafe_code(); // Save to the same file every time


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

void reload_save_file()
{
	if ((SaveFile.Emblems[0xB] & 0x8 && !(SaveFile.Emblems[0xB] & 0x2) && completedGammaLevels == 0) // If done with gamma hot shelter but not red mountain, and not incremented
		|| (SaveFile.Emblems[0xB] & 0x2 && completedGammaLevels == 1) // If done with red mountain and only incremented levels once
		|| (SaveFile.Emblems[0xB] & 0x1 && completedGammaLevels == 2)) // If done with windy valley and only incremented levels twice
	{
		completedGammaLevels++;
		oldGameMode = 12;
		return;
	}

	completedGammaLevels = 0;

	reload_message_displayed = true;
	reload_message_timer = 0;

	memcpy(&SaveFile, save_file_data, sizeof(SaveFileData));
	write_to_dest_save_file();
	LoadSave();
}

DataPointer(uint8_t, CurrentMusic, 0x912698);
//CurrentMusic == MusicIDs_TitleScreen && CurrentMusic != oldMusic
void onFrame_quick_save_reload()
{
	if (init && oldGameMode != 12 && GameMode == 12)
	{
		if (is_force_reload_enabled) 
		{
			reload_save_file();
		}
		else
		{
			reload_message_displayed = true;
			reload_message_timer = 0;
		}
	}

	oldMusic = CurrentMusic;
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
		if (is_force_reload_enabled)
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
		else
		{
			if (!was_y_pressed)
			{
				if (CurrentMusic != MusicIDs_TitleScreen && CurrentMusic != 255)
					reload_message_displayed = false;

				if (ControllerPointers[0]->PressedButtons & Buttons_Y)
				{
					reload_save_file();
					was_y_pressed = true;
				}

				SetDebugFontColor(0xFF00FFAA);
				DisplayDebugString(NJM_LOCATION(1, 1), "SADX Quick Save Reload");

				DisplayDebugString(NJM_LOCATION(1, 2), "Press Y to reload save file");
			}
			else
			{
				if (reload_message_timer < RELOAD_MESSAGE_TIMER_LENGTH)
				{
					SetDebugFontColor(0xFF00FFAA);
					DisplayDebugString(NJM_LOCATION(1, 1), "SADX Quick Save Reload");

					DisplayDebugString(NJM_LOCATION(1, 2), "Reloaded save file");

					reload_message_timer++;
				}
				else
				{
					was_y_pressed = false;
					reload_message_displayed = false;
				}
			}
		}
	}
}

bool load_save_file_data()
{
	save_file_data = (SaveFileData*) malloc(sizeof(SaveFileData));
	char* buffer = (char*) malloc(sizeof(SaveFileData));
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

static __declspec(naked) void inject_asm()
{
	__asm
	{
		push ecx

		mov ecx, esi
		add ecx, 0x12
		mov byte ptr [ecx], 01

		add ecx, 1
		mov byte ptr [ecx], 02

		pop ecx

		push 0x7DDE60
		
		push 0xDEADBEEF // 5 Byte offset for jump instruction
	}
}
void inject_failsafe_code()
{
	char numchar1 = (char) (48 + save_num / 10); // Calculate ASCII character for the decimal digits
	char numchar2 = (char) (48 + save_num % 10);

	WriteData((void*) (reinterpret_cast<int>(&inject_asm) + 0x08), &numchar1, 1);
	WriteData((void*) (reinterpret_cast<int>(&inject_asm) + 0x0E), &numchar2, 1);
	WriteJump((void*) (reinterpret_cast<int>(&inject_asm) + 0x15), (void*) 0x422160);

	WriteJump((void*) 0x42215B, &inject_asm);
}
