#pragma once

#include <windows.h>
#include <functional>

DWORD WINAPI hook_setCursorPos(std::function<void(int&, int&)> trampoline);
typedef BOOL(__stdcall * setCursorPos) (_In_ int X, _In_ int Y);
