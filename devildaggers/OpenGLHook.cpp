#pragma once

#include <windows.h>
#include <iostream>
#include <functional>

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "MinHook.h"
#include "OpenGLHook.h"


#pragma region GraphicsHooks

wglSwapBuffers _o_wglSwapBuffers;
std::function<void(HDC)> _trampoline;

BOOL __stdcall _trampoline_wglSwapBuffers(_In_ HDC  hdc)
{
	_trampoline(hdc);
	return _o_wglSwapBuffers(hdc); //return execution to original function
}

DWORD WINAPI hook_opengl(std::function<void(HDC)> trampoline)
//template<typename Class, typename T>
//DWORD WINAPI hook_opengl(MemFunPtr<Class, T> trampoline)
{
	_trampoline = trampoline;

	HMODULE hMod = GetModuleHandle(L"opengl32.dll");
	if (hMod)
	{
		std::cout << "Found OpenGL \n";
		//use GetProcAddress to find address of wglSwapBuffers in opengl32.dll
		void* ptr = GetProcAddress(hMod, "wglSwapBuffers");
		MH_Initialize();
		if (MH_CreateHook(ptr, _trampoline_wglSwapBuffers, reinterpret_cast<void**>(&_o_wglSwapBuffers)) != MH_OK)
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
//TODO: unhookOpenGL()

#pragma endregion

#pragma region InputHooks

bool _log_keyboard;

HWND EVENTS_HWND;
HHOOK MOUSE_HOOK; // handle to the hook
MSG MOUSE_MSG; // struct with information about all messages in our queue

HHOOK KEYBOARD_HOOK; // handle to the hook
MSG KEYBOARD_MSG; // struct with information about all messages in our queue

int FindExtraWindowHeight(HWND h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RECT w, c;
	GetWindowRect(h, &w);
	GetClientRect(h, &c);
	return (w.bottom - w.top) - (c.bottom - c.top);
}

LRESULT WINAPI _mouse_callback(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSLLHOOKSTRUCT * pMouseStruct = (MSLLHOOKSTRUCT *)lParam; // WH_MOUSE_LL struct
	/*
	nCode, this parameters will determine how to process a message
	This callback in this case only have information when it is 0 (HC_ACTION): wParam and lParam contain info

	wParam is about WINDOWS MESSAGE, in this case MOUSE messages.
	lParam is information contained in the structure MSLLHOOKSTRUCT
	*/

	RECT windowPos;
	GetClientRect(EVENTS_HWND, (LPRECT)&windowPos);
	ClientToScreen(EVENTS_HWND, (LPPOINT)&windowPos.left);
	ClientToScreen(EVENTS_HWND, (LPPOINT)&windowPos.right);
	windowPos.top -= FindExtraWindowHeight(EVENTS_HWND);

	if (nCode == 0)  // we have information in wParam/lParam ? If yes, let's check it:
	{
		if (ImGui::GetCurrentContext() != NULL)
		{
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			if (io.WantCaptureMouse)
			{
				if (pMouseStruct != NULL)
				{
					io.MousePos.x = pMouseStruct->pt.x - windowPos.left;
					io.MousePos.y = pMouseStruct->pt.y - windowPos.top;
				}

				switch (wParam)
				{

				case WM_LBUTTONUP:
					io.MouseClickedPos[0].x = pMouseStruct->pt.x - windowPos.left;
					io.MouseClickedPos[0].y = pMouseStruct->pt.y - windowPos.top;
					io.MouseReleased[0] = true;
					io.MouseDown[0] = false;

					break;
				case WM_LBUTTONDOWN:
					io.MouseClickedPos[0].x = pMouseStruct->pt.x - windowPos.left;
					io.MouseClickedPos[0].y = pMouseStruct->pt.y - windowPos.top;
					io.MouseDown[0] = true;
					io.MouseReleased[0] = false;
					break;

				}
			}
		}
	}

	/*
	Every time that the nCode is less than 0 we need to CallNextHookEx:
	-> Pass to the next hook
			MSDN: Calling CallNextHookEx is optional, but it is highly recommended;
			otherwise, other applications that have installed hooks will not receive hook notifications and may behave incorrectly as a result.
	*/
	return CallNextHookEx(MOUSE_HOOK, nCode, wParam, lParam);
}

LRESULT WINAPI _keyboard_callback(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT * pKeyboardStruct = (KBDLLHOOKSTRUCT *)lParam; // WH_KEYBOARD_LL struct
	if (ImGui::GetCurrentContext() != NULL)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//if (_log_keyboard) std::cout << "io.WantCaptureKeyboard: " << io.WantCaptureKeyboard << "\n";
		//not working, always 0 for some reason...
		//if (io.WantCaptureKeyboard)
		//{
			if (pKeyboardStruct != NULL)
			{
				switch (wParam)
				{
					case WM_KEYDOWN:
					case WM_SYSKEYDOWN:
						io.KeysDown[pKeyboardStruct->vkCode] = 1;
						if (_log_keyboard) std::cout << "key_down: " << pKeyboardStruct->vkCode << "\n";
						break;
					case WM_KEYUP:
					case WM_SYSKEYUP:
						io.KeysDown[pKeyboardStruct->vkCode] = 0;
						if (_log_keyboard) std::cout << "key_up: " << pKeyboardStruct->vkCode << "\n";
						break;
				}
			}
		//}
	}

	if(_log_keyboard) std::cout << "wParam: " << wParam << "\n";
	if (_log_keyboard) std::cout << "pKeyboardStruct->vkCode: " << pKeyboardStruct->vkCode << "\n";


	/*
	Every time that the nCode is less than 0 we need to CallNextHookEx:
	-> Pass to the next hook
			MSDN: Calling CallNextHookEx is optional, but it is highly recommended;
			otherwise, other applications that have installed hooks will not receive hook notifications and may behave incorrectly as a result.
	*/
	return CallNextHookEx(MOUSE_HOOK, nCode, wParam, lParam);
}

void set_hook_hwnd(HWND hwnd)
{
	EVENTS_HWND = hwnd;
}

void hook_mouse(HWND hwnd)
{
	if (!(MOUSE_HOOK = SetWindowsHookEx(WH_MOUSE_LL, _mouse_callback, NULL, 0))) {
		printf_s("Error: %x \n", GetLastError());
	}
}

void handle_mouse_events()
{
	if (PeekMessage(&MOUSE_MSG, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&MOUSE_MSG);
		DispatchMessage(&MOUSE_MSG);
	}
}

void hook_keyboard(HWND hwnd)
{
	if (!(KEYBOARD_HOOK = SetWindowsHookEx(WH_KEYBOARD_LL, _keyboard_callback, NULL, 0))) {
		printf_s("Error: %x \n", GetLastError());
	}
}

void handle_keyboard_events(bool log_keyboard)
{
	_log_keyboard = log_keyboard;
	if (PeekMessage(&KEYBOARD_MSG, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&KEYBOARD_MSG);
		DispatchMessage(&KEYBOARD_MSG);
	}
}
#pragma endregion