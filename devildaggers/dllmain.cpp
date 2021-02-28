#pragma once

//IMPORTANT: if OpenGL32.Lib is missing get it from C:\Program Files\Microsoft SDKs\Windows\v6.1\Lib\x64

#include <windows.h>
#include <Psapi.h>
#include <winternl.h>

#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"

#include <GL/glew.h>    // Initialize with glewInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include "MinHook.h"

#include "Vector3.h"
#include "GameModel.cpp"

#include "dllmain.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

HWND hwnd_game;
GLFWwindow* glfwWindow;

bool glewFailedToInitialize = false;
bool imguiInitialized = false;
bool imguiInitializedPos = false;
bool imguiInitDecollapsedHeaders = false;
bool contextCreated = false;
HGLRC thisContext;

CPlayer *player;
int cache_gems = 0;
float cache_timer = 0;
int cache_enemiesCounter = 0;

char textInputBuffer_level[32] = "";
bool changeLevel = false;

void write(void *addr, int value, const int bytes)
{
	DWORD old_protect, old_protect2;
	VirtualProtect(addr, bytes, PAGE_EXECUTE_READWRITE, &old_protect);
	memset(addr, value, bytes);
	VirtualProtect(addr, bytes, old_protect, &old_protect2);
}

void NOP( void *addr, const int bytes ) 
{
	write(addr, 0x90, bytes);
}

static void glfw_error_callback(int error, const char* description)
{
	if (error == 65537)
	{
		if(!glewFailedToInitialize)
			std::cout << "Glfw Error " << error << ": " << description << "\n";
		glewFailedToInitialize = true;
	}
	else
	{
		std::cout << "Glfw Error " << error << ": " << description << "\n";
	}
}

int FindExtraWindowHeight(HWND h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RECT w, c;
	GetWindowRect(h, &w);
	GetClientRect(h, &c);
	return (w.bottom - w.top) - (c.bottom - c.top);
}

void render(HDC hdc)
{
	//HGLRC oldContext = wglGetCurrentContext();
	if (glewFailedToInitialize)
		return;

	if (!imguiInitialized) 
	{
		std::cout << "ImGui Initializing..." << "\n";
		glfwSetErrorCallback(glfw_error_callback);

		//std::cout << "_GLFW_WNDCLASSNAME: " << _GLFW_WNDCLASSNAME << "\n";

		std::cout << "ImGui calling glewInit()" << "\n";
		//glewExperimental = GL_TRUE;
		GLenum err_glew = glewInit();
		if (err_glew != GLEW_OK)
		{
			std::cout << "glewInit() failed!" << glewGetErrorString(err_glew) << std::endl;
			glewFailedToInitialize = true;
			return;
		}
		
		std::cout << "GLFW Version:" << glfwGetVersionString() << "\n";

		IMGUI_CHECKVERSION();
		ImGuiContext* context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui_ImplWin32_Init(hwnd_game);
		ImGui_ImplOpenGL3_Init();

		ImGui::StyleColorsDark();

		io.DeltaTime = 1.0f / 60.0f;

		//io.WantCaptureKeyboard(true);

		//io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF("core/goethebold.ttf", 20.f);
		
		unsigned char * pixels;
		int width, height, bytes_per_pixels;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixels);

		imguiInitialized = true;
		std::cout << "ImGui Finished initialization" << "\n";
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//update display size based on game window (when user changes from fullscreen to window or if resizes window)
	RECT rect;
	if (GetWindowRect(hwnd_game, &rect))
	{
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		io.DisplaySize.x = width;
		io.DisplaySize.y = height;
	}

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

#pragma region RenderStuff
	if (ImGui::Begin("FuckTheDevil"))
	{
		ImGui::SetWindowPos(ImVec2(0, 100), ImGuiCond_Once);
		ImGui::SetWindowSize(ImVec2(200, 450), ImGuiCond_Once);

		//ImGui::CaptureMouseFromApp(true);
		//ImGui::CaptureKeyboardFromApp(true);

		if (ImGui::CollapsingHeader("Stats"))
		{
			ImGui::Text("FPS: %f", io.Framerate); //TODO: get it from the game not from IMGUI
			ImGui::Text("Gems: %d", cache_gems);
			ImGui::Text("Score: %f", cache_timer);
			ImGui::Text("Enemies: %d", cache_enemiesCounter);
		}

		if (ImGui::CollapsingHeader("Settings"))
		{
			ImGui::InputInt("FOV", &player->iFOV, 1);
			ImGui::Checkbox("Alive", &player->bAlive);
			ImGui::InputText("Level", textInputBuffer_level, 20, ImGuiInputTextFlags_EnterReturnsTrue);
			if (ImGui::Button("Cheat main menu"))
				changeLevel = true;
		} 

		if (!imguiInitDecollapsedHeaders)
		{
			imguiInitDecollapsedHeaders = true;
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("Stats"), 1);
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("Settings"), 1);
		}
	}
	else
	{
		std::cout << "ImGui error rendering stuff..." << "\n";
	}
	ImGui::End();
#pragma endregion

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//==========================================================================================================================

//Typedef the function prototype straight from MSDN
typedef BOOL(__stdcall * wglSwapBuffers) (_In_ HDC  hdc);

//Create instance of function
wglSwapBuffers o_wglSwapBuffers;
BOOL __stdcall trampoline_wglSwapBuffers(_In_ HDC  hdc)
{
	render(hdc);
	return o_wglSwapBuffers(hdc); //return execution to original function
}

DWORD WINAPI hookOpenGL()
{
	HMODULE hMod = GetModuleHandle(L"opengl32.dll");
	if (hMod)
	{
		std::cout << "Found OpenGL \n";

		//use GetProcAddress to find address of wglSwapBuffers in opengl32.dll
		void* ptr = GetProcAddress(hMod, "wglSwapBuffers");
		MH_Initialize();
		if (MH_CreateHook(ptr, trampoline_wglSwapBuffers, reinterpret_cast<void**>(&o_wglSwapBuffers)) != MH_OK)
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
//==========================================================================================================================


HHOOK hook; // handle to the hook
MSG msg; // struct with information about all messages in our queue
LRESULT WINAPI MouseCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	MSLLHOOKSTRUCT * pMouseStruct = (MSLLHOOKSTRUCT *)lParam; // WH_MOUSE_LL struct
	/*
	nCode, this parameters will determine how to process a message
	This callback in this case only have information when it is 0 (HC_ACTION): wParam and lParam contain info

	wParam is about WINDOWS MESSAGE, in this case MOUSE messages.
	lParam is information contained in the structure MSLLHOOKSTRUCT
	*/
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	RECT windowPos;
	GetClientRect(hwnd_game, (LPRECT)&windowPos);
	ClientToScreen(hwnd_game, (LPPOINT)&windowPos.left);
	ClientToScreen(hwnd_game, (LPPOINT)&windowPos.right);
	windowPos.top -= FindExtraWindowHeight(hwnd_game);

	if (nCode == 0)  // we have information in wParam/lParam ? If yes, let's check it:
	{
		if (pMouseStruct != NULL) 
		{ 
			io.MousePos.x = pMouseStruct->pt.x - windowPos.left;
			io.MousePos.y = pMouseStruct->pt.y - windowPos.top;
		}

		switch (wParam) {

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

	/*
	Every time that the nCode is less than 0 we need to CallNextHookEx:
	-> Pass to the next hook
		 MSDN: Calling CallNextHookEx is optional, but it is highly recommended;
		 otherwise, other applications that have installed hooks will not receive hook notifications and may behave incorrectly as a result.
	*/
	return CallNextHookEx(hook, nCode, wParam, lParam);

}
void hookMouse()
{
	if (!(hook = SetWindowsHookEx(WH_MOUSE_LL, MouseCallback, NULL, 0))) {
		printf_s("Error: %x \n", GetLastError());
	}
}

static unsigned long __stdcall CheatMain( void *arg ) {
	Beep( 500, 350 );
	
	AllocConsole();
	ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
	_iobuf *data;
	freopen_s( &data, "CONIN$", "r", stdin );
	freopen_s( &data, "CONOUT$", "w", stdout );
	SetConsoleTitleA( "IM NOT FUCKING CHEATING" );
	std::this_thread::sleep_for(std::chrono::milliseconds(4000));

	//hwnd_game = GetForegroundWindow(); //TODO: get by name to get the precise window and not the focussed one, to avoid mistakes.
	hwnd_game = FindWindowA(NULL, "Devil Daggers");
	std::cout << "Game Process\n";
	std::cout << "	HWND: " << hwnd_game << "\n";

	LPSTR windowText = new CHAR[MAX_PATH];
	GetWindowTextA(hwnd_game, windowText, MAX_PATH);
	std::cout << "	Title: " << windowText << "\n";

	const auto baseAddress = reinterpret_cast<uintptr_t>( GetModuleHandleA( nullptr ) );
	if( !baseAddress )
		return 1;
	std::cout << "Game base address: 0x" << std::hex << std::uppercase << baseAddress << "\n";

	const auto player_addr = *(DWORD_PTR*)(baseAddress + 0x226BD0) + 0x18C;
	std::cout << "CPlayer address 0x" << std::hex << std::uppercase << player_addr << "\n\n";

	//WTF? no idea of what was this supposed to do in v1 hack, maybe cursor lock/unlock? no idea.
	// dd.exe+4C067 - 01 06 - add [esi],eax
	// dd.exe+4C074 - 8B 01 - mov eax,[ecx]
	// dd.exe+4C084 - 89 01 - mov [ecx],eax
	//NOP( (PVOID)( base + 0x4C067 ), 2 );
	//NOP( (PVOID)( base + 0x4C074 ), 2 );
	//NOP( (PVOID)( base + 0x4C084 ), 2 );

	player = reinterpret_cast< CPlayer * >( player_addr );
	
	hookOpenGL();
	hookMouse();

	std::cout << std::hex << (&player) << std::endl;

	while(true)
	{
		if (!imguiInitialized)
			continue;

		//handle mouse events
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		if (!player)
			continue;

		if (changeLevel)
		{
			changeLevel = false;
			//strcpy(player->sGameMode, textInputBuffer_level);
			strcpy_s(player->sGameMode, "SECRET");
		}
		else
		{
			strcpy_s(textInputBuffer_level, player->sGameMode);
		}

		cache_gems = player->iGems;
		cache_timer = player->fTimer;
		cache_enemiesCounter = player->iEnemiesCounter;

		//std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
	}
	

	return 0;
}
int __stdcall DllMain( const HMODULE self, const unsigned long reason_for_call, void *reserved ) {
	if( reason_for_call == DLL_PROCESS_ATTACH ) {
		DisableThreadLibraryCalls(self);
		CreateThread( nullptr, 0, &CheatMain, nullptr, 0, nullptr );
		return 1;
	}

	return 0;
}

