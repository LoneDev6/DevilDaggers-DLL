#pragma once

#include <iostream>

#include "MinHook.h"

#include "SetCursorPos_hook.h"


setCursorPos _o_setCursorPos;
std::function<void(int&, int&)> _proxy_setCursorPos;

void* SetCursorPos_addr;

BOOL __stdcall _trampoline__setCursorPos(_In_ int X, _In_ int Y)
{
	_proxy_setCursorPos(X, Y);
	return _o_setCursorPos(X, Y); //return execution to original function
}

DWORD WINAPI hook_setCursorPos(std::function<void(int&, int&)> proxy)
{
	_proxy_setCursorPos = proxy;

	HMODULE hMod = GetModuleHandle(L"user32.dll");
	if (hMod)
	{
		std::cout << "Found user32.dll \n";
		//use GetProcAddress to find address of setCursorPos in the dll
		SetCursorPos_addr = GetProcAddress(hMod, "SetCursorPos");
		MH_Initialize();
		if (MH_CreateHook(SetCursorPos_addr, _trampoline__setCursorPos, reinterpret_cast<void**>(&_o_setCursorPos)) != MH_OK)
		{
			std::cout << "MH_Initialize: failed \n";
		}

		if (MH_EnableHook(SetCursorPos_addr) != MH_OK)
		{
			std::cout << "MH_EnableHook: failed \n";
		}
	}

	return 1;
}

void unhook_setCursorPos()
{
	if (MH_DisableHook(SetCursorPos_addr) != MH_OK)
	{
		std::cout << "MH_DisableHook: failed \n";
		//maybe handle it somehow?
	}
}
