#include "modules.h"
#include "pch.h"

using std::string;

bool ShownMenu = false;
bool EnableFontScaling = true;
int SaveHash = -1;
int CurrentLevelOld = -1;

void ScaleDebugFont(int scale)//thanks to PkR for this code https://github.com/PiKeyAr/sadx-debug-mode/
{
	float FontScale;
	if (!EnableFontScaling) FontScale = 1.0f;
	else
	{
		if ((float)HorizontalResolution / (float)VerticalResolution > 1.33f) FontScale = floor((float)VerticalResolution / 480.0f);
		else FontScale = floor((float)HorizontalResolution / 640.0f);
	}
	SetDebugFontSize(FontScale * scale);
}

void DisplayChaoStats()
{
	if (CurrentLevel == LevelIDs_SSGarden) //checks if level is SS Chao Garden
	{
		SaveHash = SaveFile.Checksum; //sets Save Hash to current save file Checksum
		CurrentLevelOld = CurrentLevel; //sets OldCurrentLevel to CurrentLevel
		ScaleDebugFont(15); //debug font size
		SetDebugFontColor(0xFFFFFFFF); //debug font color 
		BYTE Chao0SwimGrade = *(BYTE*)0x03C888A0; //Reads chao 0 Swim Grade from memory Address
		BYTE Chao0RunGrade = *(BYTE*)0x03C888A2; //Reads chao 0 Run Grade from memory Address
		BYTE Chao1SwimGrade = *(BYTE*)0x03C890A0; //Reads chao 1 Swim Grade from memory Address
		BYTE Chao1RunGrade = *(BYTE*)0x03C890A2; //Reads chao 1 Run Grade from memory Address
		const char* grades[] = { "E", "D", "C", "B", "A", "S" }; //array for turning hex number into Strings
		if (!ShownMenu) //checks if menu has been show befroe
		{
			DisplayDebugStringFormatted(NJM_LOCATION(0, 1), "Chao 1 Swim Grade %S", grades[Chao0SwimGrade]); //prints to screen the chao Swim Grade
			DisplayDebugStringFormatted(NJM_LOCATION(0, 2), "Chao 1 Run Grade %S", grades[Chao0RunGrade]); //prints to screen the chao Run Grade
			DisplayDebugStringFormatted(NJM_LOCATION(0, 3), "Chao 2 Swim Grade %S", grades[Chao1SwimGrade]); //prints to screen the chao Swim Grade
			DisplayDebugStringFormatted(NJM_LOCATION(0, 4), "Chao 2 Run Grade %S", grades[Chao1RunGrade]); //prints to screen the chao Run Grade
			if (Controllers[0].PressedButtons & Buttons_Y) //checks if Y is pressed
			{
				WriteData((int*)0x03C8888A, (int)0x11); //Sets chao 1 name to 1
				WriteData((int*)0x03C8908A, (int)0x12); //Set chao 2 name to 2
			}
		}
	}
	if (CurrentLevelOld == LevelIDs_SSGarden && CurrentLevel != LevelIDs_SSGarden && ShownMenu == false) //checks if oldlevel is equal to ChaoGarden and CurrentLevel isnt equal to ChaoGarden and if Menu hasnt been shown
	{
		ShownMenu = true; //sets shownmenu to true
	}
	if (SaveHash != SaveFile.Checksum && GameMode == GameModes_Menu) // checks if savehash is not equal to current save hash
	{
		SaveHash = SaveFile.Checksum;
		ShownMenu = false; //sets shownmenu to false
	}
}