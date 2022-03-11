#pragma once

static char* flags;
static long flaggedFrameCounter;
static bool flagUpdated;

static long long* NewIGTFrameCounterPointer;

void init_autosplitter(char* accessibleMemory, long long* framesPointer);

static void* jumpBackPtrs[] = { (void*)0x40C6AE };
static void* callPtrs[] = { (void*)0x642640 };

static int registerBuffer[4];

static void setFlag(const char* flagText, int length);

static void startStory();
static void getEmblemLevel();


static __declspec(naked) void generalFlagController(char a1)
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
	generalFlagController(2);
}