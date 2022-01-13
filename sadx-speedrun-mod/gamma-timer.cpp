#include "modules.h"
#include "pch.h"

static char gammaFrames;
static char gammaSeconds;
static char gammaMinutes;



void init_gamma_timer(char* injectedMemory, char* accessibleMemory)
{
	PrintDebug("injectMem Pointer: %X\n", injectedMemory);
	PrintDebug("accessMem Pointer: %X\n", accessibleMemory);

	gammaFrames = 0;
	gammaSeconds = 0;
	gammaMinutes = 0;
	
	int gammaFramesPointer = (int)&gammaFrames;

	char instructions[] = { 0xFF, 0x05, gammaFramesPointer & 0xFF, (gammaFramesPointer >> 8) & 0xFF, (gammaFramesPointer >> 16) & 0xFF, (gammaFramesPointer >> 24) & 0xFF,
							0xFE, 0x0D, 0x35, 0xEF, 0xB0, 0x03 }; // Increment our own frame counter and decrement in-game frames as original code

	memcpy(injectedMemory, instructions, 12);

	WriteJump((void*) (injectedMemory + 12), (void*) 0x426095); // Go back to original function

	WriteJump((void*) 0x42608F, injectedMemory); // Inject frame increase
	WriteData<1>((void*) 0x426094, (uint8_t) 0x90); // Add extra nop to pad out memory

	WriteData((void*) 0x426028, (void*) &accessibleMemory, 4); // Write our memory pointer into padded sonic.exe memory
	WritePointer(accessibleMemory, (int) &gammaFrames); // Write gammaFrames pointer into our memory at 0x0
	WritePointer(accessibleMemory + 0x4, (int) &gammaSeconds); // Write gammaSeconds pointer into our memory at 0x4
	WritePointer(accessibleMemory + 0x8, (int) &gammaMinutes); // Write gammaMinutes pointer into our memory at 0x8
}

void run_gamma_timer()
{
	// Hud Drawing code from dissassembly 
	/*NJS_SPRITE timerFont;

	njSetTexture(&Hud_RingTimeLife_TEXLIST);

	timerFont.p.x = ViewPortWidth - 128.0;
	timerFont.p.y = 33.0;
	timerFont.tlist = &Hud_RingTimeLife_TEXLIST;
	timerFont.tanim = &Hud_RingTimeLife_TEXANIM;
	timerFont.sx = 1.0;
	timerFont.sy = 1.0;

	SetMaterialAndSpriteColor_Float(1.0, 1.0, 1.0, 1.0);

	njDrawSprite2D_Queue(&timerFont, TimeSeconds / 10, 22046.498, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, QueuedModelFlagsB_SomeTextureThing);
	timerFont.p.x += 16.0;
	njDrawSprite2D_Queue(&timerFont, TimeSeconds % 10, 22046.498, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, QueuedModelFlagsB_SomeTextureThing);
	*/

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
			gammaSeconds = 0;
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