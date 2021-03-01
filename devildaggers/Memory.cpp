#pragma once

#include <windows.h>

void write(void *addr, int value, const int bytes)
{
	DWORD old_protect, old_protect2;
	VirtualProtect(addr, bytes, PAGE_EXECUTE_READWRITE, &old_protect);
	memset(addr, value, bytes);
	VirtualProtect(addr, bytes, old_protect, &old_protect2);
}

void NOP(void *addr, const int bytes)
{
	write(addr, 0x90, bytes);
}
