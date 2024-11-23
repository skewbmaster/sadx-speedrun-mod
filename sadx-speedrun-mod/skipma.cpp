#include "pch.h"
#include "extensions-to-modloader.h"

const char* IGJumpBackTrue = (char*) 0x431523;
const char* IGJumpBackFalse = (char*) 0x43154A;
const char* PRJumpBack = (char*) 0x5132BA;
const char& Controller0Ptr = *(char*) 0x3B0E77C;
const char& Heldbuttons0Ptr = *(char*) 0x3B0E3A8;

static __declspec(naked) void inGameCheck_asm()
{
	__asm
	{
		// OR the controller's pressed buttons variable with start, fixes all random crashing
		mov ecx, dword ptr [Controller0Ptr]
		mov ecx, dword ptr [ecx]
		push eax
		mov al, [ecx+0x10]
		or al, 8
		mov [ecx+0x10], al
		pop eax
		// Compare start like the game, but do it from held buttons
		mov ecx, dword ptr [Heldbuttons0Ptr]
		test cl, 8
		jz jumpbackfalse
		jmp IGJumpBackTrue
	jumpbackfalse:
		jmp IGJumpBackFalse
	}
}

static __declspec(naked) void preRenderedCheck_asm()
{
	__asm
	{
		mov eax, dword ptr[Heldbuttons0Ptr]
		test al, 0xC
		jmp PRJumpBack
	}
}

void SkipmaFix()
{
	// Memory for the opcodes in the fix, and 10 more bytes for jumps
	//uint32_t memoryAllocSize = 256;
	/*char* fixMemory = (char*)malloc(memoryAllocSize);
	if (!fixMemory) {
		return;
	}
	memset(fixMemory, 0x90, memoryAllocSize);*/

	PrintDebug("Address to asm: %X\n", &preRenderedCheck_asm);

	// In-game cutscene skip
	WriteJump((void*)0x431517, inGameCheck_asm);
	WriteNop<7>((void*)0x43151C);

	// Pre-rendered cutscene skip
	WriteJump((void*)0x5132B1, preRenderedCheck_asm);
	WriteNop<4>((void*)0x5132B6);
}