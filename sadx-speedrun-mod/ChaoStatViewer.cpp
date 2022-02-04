#include "modules.h"
#include "pch.h"

using std::string;

bool showmenu = false;
bool EnableFontScaling = true;
char* jumpBackAddress;

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

static __declspec(naked) void chaoFileCreated_asm()
{
	__asm
	{
		push edx
		mov edx, 1
		mov byte ptr[showmenu], dl
		pop edx
		add esp, 8
		mov esi, eax
		jmp jumpBackAddress
	}
}
void displayChaoStatsInit()
{
	jumpBackAddress = (char*)0x71ADD3;
	showmenu = false;
	WriteJump((void*)0x71ADCE, &chaoFileCreated_asm);
}

void DisplayChaoStats()
{
	if (showmenu) //checks if garden has been show befroe
	{
		ScaleDebugFont(15); //debug font size
		SetDebugFontColor(0xFFFFFFFF); //debug font color 
		BYTE Chao0SwimGrade = *(BYTE*)0x03C888A0; //Reads chao 0 Swim Grade from memory Address
		BYTE Chao0RunGrade = *(BYTE*)0x03C888A2; //Reads chao 0 Run Grade from memory Address
		BYTE Chao1SwimGrade = *(BYTE*)0x03C890A0; //Reads chao 1 Swim Grade from memory Address
		BYTE Chao1RunGrade = *(BYTE*)0x03C890A2; //Reads chao 1 Run Grade from memory Address
		const char* grades[] = { "E", "D", "C", "B", "A", "S" }; //array for turning hex number into Strings
		DisplayDebugStringFormatted(NJM_LOCATION(0, 1), "Chao 1 Swim Grade %S", grades[Chao0SwimGrade]); //prints to screen the chao Swim Grade
		DisplayDebugStringFormatted(NJM_LOCATION(0, 2), "Chao 1 Run Grade %S", grades[Chao0RunGrade]); //prints to screen the chao Run Grade
		DisplayDebugStringFormatted(NJM_LOCATION(0, 3), "Chao 2 Swim Grade %S", grades[Chao1SwimGrade]); //prints to screen the chao Swim Grade
		DisplayDebugStringFormatted(NJM_LOCATION(0, 4), "Chao 2 Run Grade %S", grades[Chao1RunGrade]); //prints to screen the chao Run Grade
		if (Controllers[0].PressedButtons & Buttons_Y) //checks if Y is pressed
		{
			WriteData((int*)0x03C8888A, (int)0x11); //Sets chao 1 name to 1
			WriteData((int*)0x03C8908A, (int)0x12); //Set chao 2 name to 2
		}
		if (CurrentLevel != LevelIDs_SSGarden)
		{
			showmenu = false;
		}
	}
}