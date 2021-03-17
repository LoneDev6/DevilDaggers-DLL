#pragma once

#include <windows.h>
#include <iostream>
#include <functional>

#include "MinHook.h"
#include "OpenGLHook.h"

wglSwapBuffers _o_wglSwapBuffers;
std::function<void(HDC)> _proxy_wglSwapBuffers;

void* wglSwapBuffers_addr;

BOOL __stdcall _trampoline_wglSwapBuffers(_In_ HDC  hdc)
{
	_proxy_wglSwapBuffers(hdc);
	return _o_wglSwapBuffers(hdc); //return execution to original function
}

DWORD WINAPI hook_opengl(std::function<void(HDC)> proxy)
{
	_proxy_wglSwapBuffers = proxy;

	HMODULE hMod = GetModuleHandle(L"opengl32.dll");
	if (hMod)
	{
		std::cout << "Found opengl32.dll \n";
		//use GetProcAddress to find address of wglSwapBuffers in the dll
		wglSwapBuffers_addr = GetProcAddress(hMod, "wglSwapBuffers");
		MH_Initialize();
		if (MH_CreateHook(wglSwapBuffers_addr, _trampoline_wglSwapBuffers, reinterpret_cast<void**>(&_o_wglSwapBuffers)) != MH_OK)
		{
			std::cout << "MH_Initialize: failed \n";
		}

		if (MH_EnableHook(wglSwapBuffers_addr) != MH_OK)
		{
			std::cout << "MH_EnableHook: failed \n";
		}
	}

	return 1;
}

void unhook_opengl()
{
	if (MH_DisableHook(wglSwapBuffers_addr) != MH_OK)
	{
		std::cout << "MH_DisableHook: failed \n";
		//maybe handle it somehow?
	}
}