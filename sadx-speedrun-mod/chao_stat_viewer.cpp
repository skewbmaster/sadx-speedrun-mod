#include "modules.h"
#include "pch.h"

using std::string;

bool showmenu = false;
bool EnableFontScaling = true;
char* jumpBackAddress;

const char* grades[] = { "E", "D", "C", "B", "A", "S" }; //array for turning hex number into Strings


void ScaleDebugFontChao(int scale, bool enableFontScaling)//thanks to PkR for this code https://github.com/PiKeyAr/sadx-debug-mode/
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

void DisplayChaoStatsOnFrame()
{
	if (showmenu) // Only does logic when true
	{
		ScaleDebugFontChao(15, EnableFontScaling); //debug font size
		SetDebugFontColor(0xFFFFFFFF); //debug font color 

		// Check if current held object's main function is chao_main
		if (Current_CharObj2 != NULL && Current_CharObj2->ObjectHeld != NULL && Current_CharObj2->ObjectHeld->MainSub == (ObjectFuncPtr)0x7202D0)
		{
			ChaoDataBase* currentHeldChaoPointer = ((ChaoData1*)(Current_CharObj2->ObjectHeld->Data1))->ChaoDataBase_ptr; // Get the chao's data pointer

			uint8_t swimGrade = currentHeldChaoPointer->SwimGrade; // Get the grades from the data pointer
			uint8_t runGrade = currentHeldChaoPointer->RunGrade;

			DisplayDebugStringFormatted(NJM_LOCATION(0, 1), "Chao Swim Grade %S", grades[swimGrade]); //prints to screen the chao Swim Grade
			DisplayDebugStringFormatted(NJM_LOCATION(0, 2), "Chao Run Grade %S", grades[runGrade]); //prints to screen the chao Run Grade
		}

		if (CurrentLevel != LevelIDs_SSGarden) // Once you leave the garden it'll disable menu drawing until you reset
		{
			showmenu = false;
		}
	}
}
