#pragma once

#include <windows.h>
#include <functional>

typedef BOOL(__stdcall * wglSwapBuffers) (_In_ HDC  hdc);
DWORD WINAPI hook_opengl(std::function<void(HDC)> trampoline);