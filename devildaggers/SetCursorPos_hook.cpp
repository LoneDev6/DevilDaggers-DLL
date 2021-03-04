#pragma once

#include <iostream>

#include "MinHook.h"

#include "SetCursorPos_hook.h"


setCursorPos _o_setCursorPos;
std::function<void(int&, int&)> _proxy_setCursorPos;

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
		void* ptr = GetProcAddress(hMod, "SetCursorPos");
		MH_Initialize();
		if (MH_CreateHook(ptr, _trampoline__setCursorPos, reinterpret_cast<void**>(&_o_setCursorPos)) != MH_OK)
		{
			std::cout << "MH_Initialize: failed \n";
		}

		if (MH_EnableHook(ptr) != MH_OK)
		{
			std::cout << "MH_EnableHook: failed \n";
		}
	}

	return 1;
}
