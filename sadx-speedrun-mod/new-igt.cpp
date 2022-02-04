#include "pch.h"
#include "modules.h"

static long long NewIGTFrameCounter;

static long long totalRTATime;
static long long currentRTATime;
static std::chrono::high_resolution_clock::time_point rtaTimeStart;
static bool timingRTA;

char oldGameMode;
int creditsCounter;

#include "new_igt_autosplitter.h"
#include "new_igt_asm.h"


void init_new_igt(char* accessibleMemory)
{
	NewIGTFrameCounter = 0;
	totalRTATime = 0;
	currentRTATime = 0;
	timingRTA = false;

	oldGameMode = GameMode;
	creditsCounter = -1;

	WriteJump((void*) 0x42F044, &beforeMoviePlay);
	WriteNop<2>((void*) 0x42F049);
	WriteJump((void*) 0x42F05A, &afterMoviePlay);
	WriteNop<5>((void*) 0x42F05F);

	WriteJump((void*) 0x4380BC, &incrementFromCameraMain);
	WriteNop<1>((void*) 0x4380C1);

	WriteJump((void*) 0x40CC62, &incrementFromOtherFrameCounter);
	WriteNop<2>((void*) 0x40CC67);
	WriteNop<6>((void*) 0x40CC71);

	char igtSignature[] = { 0xDE, 0xAD, 0xBE, 0xEF };
	memcpy(accessibleMemory, igtSignature, 4);
	WritePointer(accessibleMemory + 0x04, reinterpret_cast<int>(&NewIGTFrameCounter));
	WritePointer(accessibleMemory + 0x08, reinterpret_cast<int>(&totalRTATime));
	WritePointer(accessibleMemory + 0x0C, reinterpret_cast<int>(&currentRTATime));
	WritePointer(accessibleMemory + 0x10, reinterpret_cast<int>(&timingRTA));

	init_autosplitter(accessibleMemory, &NewIGTFrameCounter);

	PrintDebug("IGT Initialised\n");
}

void get_RTA_time()
{
	if (timingRTA)
	{
		currentRTATime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - rtaTimeStart).count();
	}
	else
	{
		currentRTATime = 0;
	}
}

void start_RTA_timer()
{
	rtaTimeStart = std::chrono::high_resolution_clock::now();

	timingRTA = true;

	PrintDebug("Starting RTA Timer\n");
}

void stop_RTA_timer()
{
	totalRTATime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - rtaTimeStart).count();

	timingRTA = false;

	PrintDebug("Stopping RTA Timer\n");
}