#pragma once
#include "stdafx.h"
#include "mem.h"
//https://guidedhacking.com/
//Credits to Solaire!
bool mem::Hook(void * toHook, void * ourFunct, int len)
{
	if (len < 5)
	{
		return false;
	}

	DWORD  curProtection;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(toHook, 0x90, len);

	uintptr_t  relativeAddress = ((uintptr_t)ourFunct - (uintptr_t)toHook) - 5;

	*(BYTE*)toHook = 0xE9;
	*(uintptr_t *)((uintptr_t)toHook + 1) = relativeAddress;

	DWORD  temp;
	VirtualProtect(toHook, len, curProtection, &temp);

	return true;
}

void * mem::TrampolineHook(void * toHook, void * ourFunct, int len)
{
	// Make sure the length is greater than 5
	if (len < 5) {
		return NULL;
	}

	// Create the gateway (len + 5 for the overwritten bytes + the jmp)
	void * gateway = VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// Put the bytes that will be overwritten in the gateway
	memcpy(gateway, toHook, len);

	// Get the gateway to destination addy
	uintptr_t  gateJmpAddy = ((uintptr_t)toHook - (uintptr_t)gateway) - 5;

	// Add the jmp opcode to the end of the gateway
	*(BYTE*)((uintptr_t)gateway + len) = 0xE9;

	// Add the address to the jmp
	*(uintptr_t *)((uintptr_t)gateway + len + 1) = gateJmpAddy;

	// Place the hook at the destination
	Hook(toHook, ourFunct, len);

	return gateway;
}