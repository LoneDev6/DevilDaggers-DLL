#pragma once

#include <windows.h>
#include "main.h"

int __stdcall DllMain( const HMODULE self, const unsigned long reason_for_call, void *reserved ) {
	if( reason_for_call == DLL_PROCESS_ATTACH ) {
		save_dll_module(self);
		DisableThreadLibraryCalls(self);
		HANDLE hThread = CreateThread(nullptr, 0, &main_thread, nullptr, 0, nullptr);
		CloseHandle(hThread);
	}
	return TRUE;
}