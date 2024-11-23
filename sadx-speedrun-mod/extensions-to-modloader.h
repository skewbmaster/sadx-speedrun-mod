#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

template <int count>
static inline BOOL WriteNop(void* writeaddress)
{
	return WriteData<count>(writeaddress, 0x90);
}

static inline BOOL WritePointer(void* writeaddress, int pointeraddress)
{
	return WriteData(writeaddress, (void*)&pointeraddress, sizeof(int*));
}
static inline BOOL WritePointer(void* writeaddress, void* pointer)
{
	return WriteData(writeaddress, &pointer, sizeof(int*));
}