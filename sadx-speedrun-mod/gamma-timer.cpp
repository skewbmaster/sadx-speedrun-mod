#include "modules.h"
#include "pch.h"

static char gammaFrames;
static char gammaSeconds;
static char gammaMinutes;

static __declspec(naked) void incrementGammaFrames()
{
	__asm
	{
		inc byte ptr gammaFrames
		
		mov eax, 0x3B0EF35
		dec byte ptr [eax]

		mov eax, 0x426095
		jmp eax
	}
}

static __declspec(naked) void incrementOtherCharFrames()
{
	__asm
	{
		inc byte ptr gammaFrames

		mov edx, 0x3B0EF35
		mov byte ptr [edx], al
		cmp al, 60

		mov edx, 0x426139

		jmp edx
	}
}

void init_gamma_timer(char* accessibleMemory)
{
	gammaFrames = 0;
	gammaSeconds = 0;
	gammaMinutes = 0;

	WriteJump((void*) 0x42608F, &incrementGammaFrames); // Inject frame increase into TickClock Function
	WriteNop<1>((void*) 0x426094); // Add extra nop to pad out memory

	WriteJump((void*) 0x426134, &incrementOtherCharFrames); // Inject frame increase again for every other character

	WritePointer(accessibleMemory, (int) &gammaFrames); // Write gammaFrames pointer into our memory at 0x0
	WritePointer(accessibleMemory + 0x4, (int) &gammaSeconds); // Write gammaSeconds pointer into our memory at 0x4
	WritePointer(accessibleMemory + 0x8, (int) &gammaMinutes); // Write gammaMinutes pointer into our memory at 0x8
}

void run_gamma_timer()
{
	if (GameState == 7 || (GameMode != 4 && GameMode != 9))
	{
		gammaFrames = 0;
		gammaSeconds = 0;
		gammaMinutes = 0;
	}

	if (gammaFrames >= 60)
	{
		gammaFrames -= 60;
		gammaSeconds++;

		if (gammaSeconds >= 60)
		{
			gammaSeconds -= 60;
			gammaMinutes++;

			if (gammaMinutes >= 100)
			{
				gammaFrames = 59;
				gammaSeconds = 59;
				gammaMinutes = 99;
			}
		}
	}
}