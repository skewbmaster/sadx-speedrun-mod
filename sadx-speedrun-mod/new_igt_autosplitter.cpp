#include "pch.h"

#include "new_igt_autosplitter.h"

#include <string>

#define MAX_FLAG_NAME_SIZE 64


void init_autosplitter(char* accessibleMemory, long long* framesPointer)
{
	flags = (char*) malloc(MAX_FLAG_NAME_SIZE);
	if (flags == NULL)
	{
		PrintDebug("Couldn't allocate flags");
		return;
	}

	flaggedFrameCounter = 0;
	flagUpdated = false;

	NewIGTFrameCounterPointer = framesPointer;

	WriteData<MAX_FLAG_NAME_SIZE>(flags, 0); // Write our flags with nothing so there's no confusion in external reading
	strcpy_s(flags, 10, "GameStart");
	
	WritePointer(accessibleMemory + 0x14, reinterpret_cast<int>(flags));
	WritePointer(accessibleMemory + 0x18, reinterpret_cast<int>(&flaggedFrameCounter));
	WritePointer(accessibleMemory + 0x1C, reinterpret_cast<int>(&flagUpdated));

	PrintDebug("startstoryptr: %X\n", &startStory);
	WriteJump((void*) 0x40C6A8, &startStory);
	WriteNop<1>((void*) 0x40C6AD);

	PrintDebug("yeppenguin: %X\n", &generalFlagController);
	PrintDebug("whypenguin: %X\n", &testFunc);

	// 0x4B4250 Emblem increment from level split
}

static void setFlag(const char* flagText, int length)
{
	strcpy_s(flags, length + 1, flagText);
	std::string curlvl = std::to_string(CurrentLevel);
	//curlvl.

	//strcpy_s();

	flaggedFrameCounter = *NewIGTFrameCounterPointer;
	flagUpdated = true;
}

static __declspec(naked) void startStory()
{
	__asm push edx

	setFlag("StoryStart", 10);

	__asm
	{
		pop edx
		push ebx
		call callPtrs[0]
		jmp jumpBackPtrs[0]
	}
}

static __declspec(naked) void getEmblemLevel()
{
	setFlag("Haha", 4);

	__asm
	{

	}
}
