#pragma once

static void* sonicAddressPointers[] = { (void*)0x77ED40, (void*)0x42F04B, (void*)0x42F064, (void*)0x4380C2, (void*)0x40CC69 };


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
		mov dword ptr [eax], 0
		jmp sonicAddressPointers[8] //jmp 0x42F064
	}
}

void checkCredits();

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

		skipDoubleAdd:
		inc dword ptr NewIGTFrameCounter

		skipSingleAdd:
		mov eax, 0x3B2C6C8
		mov ecx, [eax]
		inc ecx
		mov [eax], ecx
		
		jmp sonicAddressPointers[12]
	}
}
static __declspec(naked) void incrementFromOtherFrameCounter()
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
		je increment

		cmp ecx, 12
		je increment

		cmp ecx, 18
		je increment

		cmp ecx, 20
		je increment

		mov ecx, 0x3B22DE4
		mov ecx, [ecx]

		cmp ecx, 19
		je increment

		cmp ecx, 16
		je increment

		jmp jumpBack

		increment:
		inc dword ptr NewIGTFrameCounter

		jumpBack:
		mov ecx, 0x3ABDF58
		inc dword ptr [ecx]
		

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