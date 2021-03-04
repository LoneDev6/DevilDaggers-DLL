#pragma once

#include <windows.h>

void hook_mouse(HWND hwnd);
void hook_keyboard(HWND hwnd);
void handle_mouse_events();
void handle_keyboard_events(bool log_keyboard);