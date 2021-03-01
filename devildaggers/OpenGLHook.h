#pragma once

#include <windows.h>
#include <iostream>
#include <functional>

typedef BOOL(__stdcall * wglSwapBuffers) (_In_ HDC  hdc);
DWORD WINAPI hook_opengl(std::function<void(HDC)> trampoline);
void hook_mouse(HWND hwnd);
void hook_keyboard(HWND hwnd);
void handle_mouse_events();
void handle_keyboard_events(bool log_keyboard);