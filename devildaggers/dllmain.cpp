#pragma once

//IMPORTANT: if OpenGL32.Lib is missing get it from C:\Program Files\Microsoft SDKs\Windows\v6.1\Lib\x64

#define OPEN_CLOSE_KEY VK_INSERT


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
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

#include "OpenGLHook.h"
#include "Memory.h"
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

bool menu_opened = true;

bool log_keyboard = true;

bool glewFailedToInitialize = false;
bool imguiInitialized = false;
bool imguiInitializedPos = false;
bool imguiInitDecollapsedHeaders = false;

CPlayer *player;
int cache_enemiesCounter = 0;
char textInputBuffer_level[32] = "";
//bool changeLevel = false;

bool enabled_crosshair = false;

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

void render_loop(HDC hdc)
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

		io.DeltaTime = 1.0f / io.Framerate;

		io.WantCaptureKeyboard = true;
		io.WantCaptureMouse = true;

		//io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF("core/goethebold.ttf", 20.f);
		
		unsigned char * pixels;
		int width, height, bytes_per_pixels;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixels);

		imguiInitialized = true;
		std::cout << "ImGui Finished initialization" << "\n";
	}

	if (!imguiInitialized)
		return;

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (ImGui::IsKeyDown(OPEN_CLOSE_KEY))
	{
		//shit
		io.KeysDown[OPEN_CLOSE_KEY] = 0;

		std::cout << "Closed menu (released key: " << OPEN_CLOSE_KEY << ") \n";
		menu_opened = !menu_opened;

		return;
	}

	if (!menu_opened)
		return;


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

		if (ImGui::CollapsingHeader("Status"))
		{
			ImGui::Text("FPS: %f", io.Framerate); //TODO: get it from the game not from IMGUI
			ImGui::Text("Gems: %d", player->iGems);
			ImGui::Text("Score: %f", player->fTimer);
			//ImGui::Text("Enemies: %d", cache_enemiesCounter); //TODO: i have to find the address
			ImGui::Checkbox("Alive", &player->bAlive);
			//ImGui::InputText("Level", textInputBuffer_level, 20, ImGuiInputTextFlags_EnterReturnsTrue);
		}

		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::Text("X: %f", player->fCamX);
			ImGui::Text("Y: %f", player->fCamY);
			ImGui::Text("Z: %f", player->fCamZ);
			ImGui::Checkbox("Crosshair", &enabled_crosshair);
			//TODO: freecam
		}

		if (ImGui::CollapsingHeader("Other"))
		{
			ImGui::Checkbox("log_keyboard", &log_keyboard);
			//ImGui::PushItemWidth(20); //seems to bug the mouse input
			//ImGui::InputText("DD Version", player->sVersion, 6, ImGuiInputTextFlags_ReadOnly);
			ImGui::Text("DD Version");
			ImGui::Text(player->sVersion);
		} 

		if (!imguiInitDecollapsedHeaders)
		{
			imguiInitDecollapsedHeaders = true;
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("Status"), 1);
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("Camera"), 1);
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("Other"), 1);
		}
	}
	else
	{
		std::cout << "ImGui error rendering stuff..." << "\n";
	}

	//show cursor
	//ImGui::GetIO().MouseDrawCursor = true;
	//change cursor
	//ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);

	if (enabled_crosshair)
	{
		//todo move on init?
		int yOffset = 15;
		int xOffset = 0;
		int size = 6;

		//TODO: move into separate function
		ImGui::GetForegroundDrawList()->AddLine(
			*new ImVec2(io.DisplaySize.x / 2 + size + xOffset, io.DisplaySize.y / 2 + size + yOffset),
			*new ImVec2(io.DisplaySize.x / 2 - size + xOffset, io.DisplaySize.y / 2 - size + yOffset),
			0xFF0000FF
		);

		ImGui::GetForegroundDrawList()->AddLine(
			*new ImVec2(io.DisplaySize.x / 2 - size + xOffset, io.DisplaySize.y / 2 + size + yOffset),
			*new ImVec2(io.DisplaySize.x / 2 + size + xOffset, io.DisplaySize.y / 2 - size + yOffset),
			0xFF0000FF
		);
	}

	ImGui::End();
#pragma endregion

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	/*ShowCursor(true);
	SetCursor(LoadCursor(NULL, IDC_CROSS));
	glfwDisable(GLFW_MOUSE_CURSOR);
	glcursor()*/
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

	hwnd_game = FindWindowA(NULL, "Devil Daggers");
	std::cout << "Game Process:\n";
	std::cout << "	HWND: " << hwnd_game << "\n";

	LPSTR windowText = new CHAR[MAX_PATH];
	GetWindowTextA(hwnd_game, windowText, MAX_PATH);
	std::cout << "	Title: " << windowText << "\n";

	const auto baseAddress = reinterpret_cast<uintptr_t>( GetModuleHandleA( nullptr ) );
	if( !baseAddress )
		return 1;
	std::cout << "Base address: 0x" << std::hex << std::uppercase << baseAddress << "\n";

	const auto player_addr = *(DWORD_PTR*)(baseAddress + 0x226BD0) + 0x18C;
	std::cout << "CPlayer address 0x" << std::hex << std::uppercase << player_addr << "\n\n";

	player = reinterpret_cast<CPlayer*>(player_addr);

	//WTF? no idea of what was this supposed to do in v1 hack, maybe fix for freecam lag, no idea.
	// dd.exe+4C067 - 01 06 - add [esi],eax
	// dd.exe+4C074 - 8B 01 - mov eax,[ecx]
	// dd.exe+4C084 - 89 01 - mov [ecx],eax
	//NOP( (PVOID)( base + 0x4C067 ), 2 );
	//NOP( (PVOID)( base + 0x4C074 ), 2 );
	//NOP( (PVOID)( base + 0x4C084 ), 2 );

	hook_opengl(&render_loop);
	hook_mouse(hwnd_game);
	hook_keyboard(hwnd_game);

	while(true)
	{
		if (!imguiInitialized)
			continue;

		handle_mouse_events();
		handle_keyboard_events(log_keyboard);

		if (!player)
			continue;

		/*if (changeLevel)
		{
			changeLevel = false;
			//strcpy(player->sGameMode, textInputBuffer_level);
			strcpy_s(player->sLevelName, "SECRET");
		}
		else
		{
			strcpy_s(textInputBuffer_level, player->sLevelName);
		}*/

		//cache_enemiesCounter = player->iEnemiesCounter; //TODO: not working (seems now changes while shooting???)

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

