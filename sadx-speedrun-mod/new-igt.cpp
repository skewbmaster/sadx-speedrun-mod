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
	WriteNop<6>((void*) 0x4380CB);

	WriteJump((void*) 0x40CC62, &incrementFromMainFrameCounter);
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

static __declspec(naked) void beforeMoviePlay()
{
	__asm
	{
		call start_RTA_timer
		push 01
		call sonicAddressPointers[0] //call 0x77ED40
		jmp sonicAddressPointers[4] //jmp 0x42F04B
	}
}

static __declspec(naked) void afterMoviePlay()
{
	__asm
	{
		call stop_RTA_timer
		mov eax, 0x3B2C45C
		mov dword ptr[eax], 0
		jmp sonicAddressPointers[8] //jmp 0x42F064
	}
}

static __declspec(naked) void incrementFromCameraMain()
{
	__asm
	{
		mov ecx, 0x3B2C55C // inCutsceneIndicator
		mov ecx, [ecx]
		cmp ecx, 0
		je skipDoubleAdd

		add dword ptr NewIGTFrameCounter, 2
		jmp skipSingleAdd

		skipDoubleAdd :
		inc dword ptr NewIGTFrameCounter

			skipSingleAdd :
		mov eax, 0x3B2C6C8
			mov ecx, [eax]
			inc ecx
			mov[eax], ecx

			jmp sonicAddressPointers[12]
	}
}
static __declspec(naked) void incrementFromMainFrameCounter()
{
	__asm
	{
		mov cl, timingRTA
		cmp cl, 1
		je jumpBack // if timingRTA -> return

		push ebx
		push edx

		call checkCredits

		pop edx
		pop ebx

		mov ecx, 0x3ABDC7C
		mov ecx, [ecx]

		cmp ecx, 1
		je increment // if gameMode == 1 Splash Logos

		cmp ecx, 12
		je increment // if gameMode == 12 Title + Menus

		cmp ecx, 18
		je incrementScalar // if gameMode == 18 Story Introduction

		cmp ecx, 20
		je incrementScalar // if gameMode == 20 Instruction

		mov ecx, 0x3B22DE4
		mov ecx, [ecx]

		cmp ecx, 19
		je incrementScalar // if gameState == 19 Game over

		cmp ecx, 16
		je incrementScalar // if gameState == 16 Pause

		jmp jumpBack

		increment : // Normal increments
		inc dword ptr NewIGTFrameCounter
			jmp jumpBack


			incrementScalar : // Increments for different framerate settings
		mov ecx, 0x89295C
			mov ecx, [ecx]

			add dword ptr NewIGTFrameCounter, ecx

			jumpBack :
		mov ecx, 0x3ABDF58
			inc dword ptr[ecx]


			jmp sonicAddressPointers[16]
	}
}

static void checkCredits()
{
	if (GameMode == 22)
	{
		if (oldGameMode != 22)
		{
			creditsCounter = 3600; // Arbitrary delay before frames are added during credits, so that categories that skip the credits don't get added frames
		}

		creditsCounter--;

		if (creditsCounter == 0)
		{
			switch (CurrentCharacter)
			{
				case 0:
				{
					if (LastStoryFlag)
						NewIGTFrameCounter += 17360;
					else
						NewIGTFrameCounter += 16804;

					break;
				}
				case 2:
				{
					NewIGTFrameCounter += 14657;
					break;
				}
				case 3:
				{
					NewIGTFrameCounter += 16804;
					break;
				}
				case 5:
				{
					NewIGTFrameCounter += 18950;
					break;
				}
				case 6:
				{
					NewIGTFrameCounter += 17545;
					break;
				}
				case 7:
				{
					NewIGTFrameCounter += 15333;
					break;
				}
				default:
					break;
			}
		}
	}

	oldGameMode = GameMode;
}