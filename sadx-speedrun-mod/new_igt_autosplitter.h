#pragma once

static char* flags;
static long long flaggedFrameCounter;
static bool flagUpdated;

void init_autosplitter(char* accessibleMemory);

static void* jumpBackPtrs[] = { (void*)0x40C6AE };
static void* callPtrs[] = { (void*)0x642640 };

static int registerBuffer[4];

static __declspec(naked) void startStory()
{
	__asm
	{
		push ebx
		call callPtrs[0]
		push eax
		push ecx
		push edx
	}

	strcpy_s(flags, 11, "StoryStart");

	__asm
	{
		pop edx
		pop ecx
		pop eax
		jmp jumpBackPtrs[0]
	}
}


static __declspec(naked) void flagController(char a1)
{
	__asm
	{
		mov registerBuffer[0x0], eax
		mov registerBuffer[0x4], ebx
		mov registerBuffer[0x8], ecx
		mov registerBuffer[0xC], edx

		mov eax, [esp+4]
	}

	__asm
	{
goback:
		mov eax, registerBuffer[0x0]
		mov ebx, registerBuffer[0x4]
		mov ecx, registerBuffer[0x8]
		mov edx, registerBuffer[0xC]

		//jmp a1
	}
}

static __declspec(naked) void testFunc()
{
	flagController(2);
}