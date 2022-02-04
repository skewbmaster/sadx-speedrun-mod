#include "pch.h"

#include "new_igt_autosplitter.h"

#include <string>

#define MAX_FLAG_NAME_SIZE 64


void init_autosplitter(char* accessibleMemory, long long* framesPointer)
{
	flags = (char*) malloc(MAX_FLAG_NAME_SIZE);
	flaggedFrameCounter = 0;
	flagUpdated = false;

	NewIGTFrameCounterPointer = framesPointer;

	strcpy_s(flags, 10, "GameStart");
	
	WritePointer(accessibleMemory + 0x14, reinterpret_cast<int>(flags));
	WritePointer(accessibleMemory + 0x18, reinterpret_cast<int>(&flaggedFrameCounter));
	WritePointer(accessibleMemory + 0x1C, reinterpret_cast<int>(&flagUpdated));

	PrintDebug("startstoryptr: %X\n", &startStory);
	WriteJump((void*) 0x40C6A8, &startStory);
	WriteNop<1>((void*) 0x40C6AD);

	PrintDebug("yeppenguin: %X\n", &flagController);
	PrintDebug("whypenguin: %X\n", &testFunc);

	// 0x4B4250 Emblem increment from level split
}