#pragma once

#define GAME_WINDOW_TITLE "Devil Daggers"
#define INITIAL_SLEEP_MS 4000
#define OPEN_CLOSE_KEY VK_INSERT
#define OPEN_CLOSE_KEY_STR "INS"
#define UNLOCK_CURSOR_KEY VK_LSHIFT
#define UNLOCK_CURSOR_KEY_STR "LEFT SHIFT"
#define TOP_DOWN_VIEW_KEY 0x51 //Q
#define TOP_DOWN_VIEW_KEY_STR "Q"
bool LOG_KEYBOARD = false;

#define CONSOLE_TITLE "IM NOT FUCKING CHEATING"
#define IMGUI_TITLE "FuckTheDevil"
#define FONT_PATH "core/goethebold.ttf"
#define FONT_SIZE 20.f


#include <windows.h>
#include <Psapi.h>
#include <winternl.h>

#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <stdio.h>

#include "imgui/imgui.h"
//IMPORTANT: if OpenGL32.Lib is missing get it from C:\Program Files\Microsoft SDKs\Windows\v6.1\Lib\x64
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"

#include <GL/glew.h>    // Initialize with glewInit()
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

#include "OpenGLHook.h"
#include "InputHook.h"
#include "SetCursorPos_hook.h"
#include "Memory.h"
#include "GameModel.h"

#include "main.h"
// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

HWND hwnd_game;

bool menu_opened = true;

bool glfwFailedToInitialize = false;
bool imguiInitialized = false;
bool imguiInitializedPos = false;
bool imguiInitDecollapsedHeaders = false;

Hero *hero;
int cache_enemiesCounter = 0;
char textInputBuffer_level[32] = "";
bool enabled_crosshair = false;

unsigned long __stdcall main_thread(void *arg) {
	Beep(500, 350);

	AllocConsole();
	ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
	_iobuf *data;
	freopen_s(&data, "CONIN$", "r", stdin);
	freopen_s(&data, "CONOUT$", "w", stdout);
	SetConsoleTitleA(CONSOLE_TITLE);
	std::this_thread::sleep_for(std::chrono::milliseconds(INITIAL_SLEEP_MS));

	hwnd_game = FindWindowA(NULL, GAME_WINDOW_TITLE);
	std::cout << "Game Process:\n";
	std::cout << "	HWND: " << hwnd_game << "\n";

	LPSTR windowText = new CHAR[MAX_PATH];
	GetWindowTextA(hwnd_game, windowText, MAX_PATH);
	std::cout << "	Title: " << windowText << "\n";

	const auto baseAddress = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
	if (!baseAddress)
		return 1;
	std::cout << "Base address: 0x" << std::hex << std::uppercase << baseAddress << "\n";

	const auto hero_addr = baseAddress + HERO_PTR_ADDRESS;
	std::cout << "Hero address 0x" << std::hex << std::uppercase << hero_addr << "\n\n";
	hero = Hero::init(hero_addr);

	hook_opengl(&render_loop_proxy);
	hook_setCursorPos(&setCursorPos_proxy);
	hook_mouse(hwnd_game);
	hook_keyboard(hwnd_game);

	//main loop
	while (true)
	{
		if (!imguiInitialized)
			continue;

		handle_mouse_events();
		handle_keyboard_events(LOG_KEYBOARD);

		if (!hero)
			continue;

		//std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
	}

	return 0;
}

void setCursorPos_proxy(int &x, int &y)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!ImGui::IsKeyDown(UNLOCK_CURSOR_KEY))
	{
		ImGui::GetIO().MouseDrawCursor = false;
		return;
	}

	//show cursor
	ImGui::GetIO().MouseDrawCursor = true;
	
	//unlock cursor
	POINT cursor;
	GetCursorPos(&cursor);
	x = cursor.x;
	y = cursor.y;
}

void render_loop_proxy(HDC hdc)
{
	if (glfwFailedToInitialize)
		return;

	if (!imguiInitialized)
	{
		std::cout << "ImGui Initializing..." << "\n";
		glfwSetErrorCallback(glfw_error_callback);

		std::cout << "ImGui calling glewInit()" << "\n";
		//glewExperimental = GL_TRUE;
		GLenum err_glew = glewInit();
		if (err_glew != GLEW_OK)
		{
			std::cout << "glewInit() failed!" << glewGetErrorString(err_glew) << std::endl;
			glfwFailedToInitialize = true;
			return;
		}

		std::cout << "GLFW Version:" << glfwGetVersionString() << "\n";

		IMGUI_CHECKVERSION();
		ImGuiContext* context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);
		ImGuiIO& io = ImGui::GetIO();

		ImGui_ImplWin32_Init(hwnd_game);
		ImGui_ImplOpenGL3_Init();

		ImGui::StyleColorsDark();

		io.DeltaTime = 1.0f / io.Framerate;

		io.FontGlobalScale = 0.9f;

		io.WantCaptureKeyboard = true;
		io.WantCaptureMouse = true;

		//io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF(FONT_PATH, FONT_SIZE);

		unsigned char * pixels;
		int width, height, bytes_per_pixels;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixels);

		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

		imguiInitialized = true;
		std::cout << "ImGui Finished initialization" << "\n";
	}

	if (!imguiInitialized)
		return;

	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsKeyDown(OPEN_CLOSE_KEY))
	{
		io.KeysDown[OPEN_CLOSE_KEY] = 0;//shit
		std::cout << "Open/close menu" << std::endl;
		menu_opened = !menu_opened;
		return;
	}

	hero->bTopDownCamera = io.KeysDown[TOP_DOWN_VIEW_KEY];

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

#pragma region MenuEntries
	if (ImGui::Begin(IMGUI_TITLE))
	{
		ImGui::SetWindowPos(ImVec2(250, 150), ImGuiCond_Once);
		ImGui::SetWindowSize(ImVec2(260, 660), ImGuiCond_Once);

		//ImGui::CaptureMouseFromApp(true);
		//ImGui::CaptureKeyboardFromApp(true);
		ImGui::Text("DD Version: %s", hero->sGameVersion);
		if (ImGui::CollapsingHeader("Status"))
		{
			ImGui::Text("FPS: %f", io.Framerate); //TODO: get it from the game not from IMGUI
			ImGui::Text("Gems: %d", hero->iGems);
			ImGui::Text("Score: %f", hero->fSeconds);
			ImGui::Text("Tiles-shrink time: %f", hero->fTilesShrinkTime);
			ImGui::Text("Killed enemies: %d", hero->iKilledEnemies);
			//ImGui::Text("Enemies: %d", cache_enemiesCounter); //TODO: i have to find the address
			ImGui::Checkbox("Alive", &hero->bAlive);
			//ImGui::InputText("Level", textInputBuffer_level, 20, ImGuiInputTextFlags_EnterReturnsTrue);
		}

		if (ImGui::CollapsingHeader("World"))
		{
			ImGui::Text("Scene: %s", hero->sLevelName);
			ImGui::Text("Entities: %d", hero->iCounterEnemiesAndSpawners);
			ImGui::Text("SKULL1: %d", hero->iCounter_skull_1_HUGE_PEAK_BUG);
		}

		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::Text("X: %f", hero->fCamX);
			ImGui::Text("Y: %f", hero->fCamY);
			ImGui::Text("Z: %f", hero->fCamZ);
			ImGui::Checkbox("Crosshair", &enabled_crosshair);
			ImGui::Checkbox("Top-down", &hero->bTopDownCamera);
			ImGui::SliderFloat("Sky light", &hero->fSkyLight, 0, 500);
			//TODO: add a check to "auto apply" light on world change
			//TODO: freecam
		}

		if (ImGui::CollapsingHeader("Info"))
		{
			//not working
			//ImGui::Text("1st in leaderboard: %s", hero->sLeaderboardFirstUsername);
			ImGui::Text("Open/close menu: %s", OPEN_CLOSE_KEY_STR);
			ImGui::Text("Unlock cursor: hold %s", UNLOCK_CURSOR_KEY_STR);
			ImGui::Text("Top-down view: hold %s", TOP_DOWN_VIEW_KEY_STR);
			ImGui::Checkbox("log_keyboard", &LOG_KEYBOARD);
			ImGui::Text("Coded by github.com/LoneDev6", OPEN_CLOSE_KEY_STR);
			//ImGui::PushItemWidth(20); //seems to bug the mouse input
			//ImGui::InputText("DD Version", hero->sGameVersion, 6, ImGuiInputTextFlags_ReadOnly);
		}

		if (!imguiInitDecollapsedHeaders)
		{
			imguiInitDecollapsedHeaders = true;
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("Status"), 1);
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("World"), 1);
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("Camera"), 1);
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("Info"), 1);
		}
	}
	else
	{
		std::cout << "ImGui error rendering stuff..." << "\n";
	}

	if (enabled_crosshair && !hero->bTopDownCamera)
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

static void glfw_error_callback(int error, const char* description)
{
	if (error == 65537)
	{
		if (!glfwFailedToInitialize)
			std::cout << "Failed to initialize glfw: " << error << ": " << description << "\n";
		glfwFailedToInitialize = true;
	}
	else
	{
		std::cout << "Glfw error: " << error << ": " << description << "\n";
	}
}