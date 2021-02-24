#pragma once
#include "stdafx.h"
#include "windows.h"
#include <vector>
//https://guidedhacking.com/

namespace mem
{
	bool Hook(void * toHook, void * ourFunct, int len);
	void * TrampolineHook(void * toHook, void * ourFunct, int len);
};