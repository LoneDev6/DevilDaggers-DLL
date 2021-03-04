#pragma once

#include <windows.h>
#include "main.h"

int __stdcall DllMain( const HMODULE self, const unsigned long reason_for_call, void *reserved ) {
	if( reason_for_call == DLL_PROCESS_ATTACH ) {
		DisableThreadLibraryCalls(self);
		CreateThread( nullptr, 0, &main_thread, nullptr, 0, nullptr );
		return 1;
	}
	//TODO: detach
	return 0;
}