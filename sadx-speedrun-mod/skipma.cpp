#include "pch.h"
#include "extensions-to-modloader.h"

const char* IGJumpBackTrue = (char*) 0x431523;
const char* IGJumpBackFalse = (char*) 0x43154A;
const char* PRJumpBack = (char*) 0x5132BA;

static __declspec(naked) void inGameCheck_asm()
{
	__asm
	{
		// OR the controller's pressed buttons variable with start, fixes all random crashing
		mov ecx, 0x3B0E77C
		mov ecx, dword ptr [ecx]
		push eax
		mov al, [ecx+0x10]
		or al, 8
		mov [ecx+0x10], al
		pop eax
		// Compare start like the game, but do it from held buttons
		mov ecx, 0x3B0E3A8
		mov ecx, dword ptr [ecx]
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
		mov eax, 0x3B0E3A8
		mov eax, dword ptr [eax]
		test al, 0xC
		jmp PRJumpBack
	}
}

void SkipmaFix()
{
	// In-game cutscene skip
	WriteJump((void*)0x431517, inGameCheck_asm);
	WriteNop<7>((void*)0x43151C);

	// Pre-rendered cutscene skip
	WriteJump((void*)0x5132B1, preRenderedCheck_asm);
	WriteNop<4>((void*)0x5132B6);
}