#pragma once

#include <windows.h>
#include <Psapi.h>
#include <winternl.h>

#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING)
#define GLFW_INCLUDE_NONE         // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>  // Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include "dllmain.h"

#include "glText.h"
#include "glDraw.h"

//#include "Memory.h"
#include "Vector3.h"


// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int FindExtraWindowHeight(HWND h)
{
	RECT w, c;
	GetWindowRect(h, &w);
	GetClientRect(h, &c);
	return (w.bottom - w.top) - (c.bottom - c.top);
}

//#include "detours.h" //detours 3.0
//#pragma comment(lib, "detours.lib")

class CPlayer {
public:
	char pad_0000[96]; //0x0000
	char sPlayerName_idk[4]; //0x0060
	char pad_0064[92]; //0x0064
	char sVersion_idk[4]; //0x00C0
	char pad_00C4[24]; //0x00C4
	char sGameMode2_blankSometimes[4]; //0x00DC
	char pad_00E0[108]; //0x00E0
	int32_t iFOV; //0x014C
	char pad_0150[64]; //0x0150
	float fTimer; //0x0190
	char pad_0194[16]; //0x0194
	bool bAlive; //0x01A4
	char pad_01A5[27]; //0x01A5
	int32_t iGems; //0x01C0
	char pad_01C4[56]; //0x01C4
	int32_t iEnemiesCounter; //0x01FC
	char pad_0200[36]; //0x0200
	char sGameMode[5]; //0x0224
	char pad_0229[308]; //0x0229
	int8_t N000002E8; //0x035D
	bool bReplayMode; //0x035E
	char pad_035F[230]; //0x035F
	char cPlayerName[8]; //0x0445
	char pad_044D[264]; //0x044D
	Vector3 vCamera; //0x0555
	char pad_0561[748]; //0x0561
}; //Size: 0x084D


/*public:
	char pad_0000[400]; //0x0000
	float m_unk_timer; //0x0190
	char pad_0194[12]; //0x0194
	float m_score; //0x01A0
	bool m_alive; //0x01A4
	char pad_01A8[24]; //0x01A8
	uint32_t m_gems; //0x01C0 //not resetting on death
	char pad_01C4[1200]; //0x01C4
}; //Size: 0x0674
*/


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


HWND hwnd;
GLFWwindow* window;

bool imguiInitialized = false;
bool imguiInitializedPos = false;
bool imguiInitDecollapsedHeaders = false;
//GL::Font glFont;

CPlayer *player;
int cache_gems = 0;
float cache_timer = 0;
int cache_enemiesCounter = 0;

char textInputBuffer_level[32] = "";
bool changeLevel = false;
bool triggerFreecam = false;
bool bFreecam = false;

//dd.exe+1A075 - 8B 41 28 - mov eax,[ecx+28]
int originalFreecam[] = { 0x8B, 0x41, 0x28 };

void OpenGL_loop()
{
	/*HDC currentHDC = wglGetCurrentDC();
	if (!glFont.bBuilt || currentHDC != glFont.hdc)
		glFont.Build(50);*/
	//GL::SetupOrtho();
	//GL::DrawFilledRect(0, 0, 500, 500, rgb::red);
	//GL::Text(currentHDC, 50, 50, 255, 255, 255, GLUT_BITMAP_HELVETICA_18, "TESTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
	//glFont.Print(10, 50, rgb::red, "TESTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	hwnd = GetActiveWindow();

	//https://www.unknowncheats.me/forum/c-and-c-/291229-imgui-opengl3-menu-help.html
	if (!imguiInitialized) 
	{
		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

		glewInit();

		RECT rect;
		if (GetWindowRect(hwnd, &rect))
		{
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			window = glfwCreateWindow(width, height, "ImGui", NULL, NULL);
		}
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable vsync

		glfwSetErrorCallback(glfw_error_callback);

		IMGUI_CHECKVERSION();
		ImGuiContext* context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		//ImGui_ImplWin32_Init(hwnd);

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		io.DeltaTime = 1.0f / 60.0f;
		//io.DisplaySize.x = 1920;
		//io.DisplaySize.y = 1080;
		//io.WantCaptureKeyboard(true);

		//io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF("core/goethebold.ttf", 20.f);
		
		unsigned char * pixels;
		int width, height, bytes_per_pixels;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixels);

		ImGui::StyleColorsDark();

		ImGui_ImplOpenGL3_Init();

		//ImGui_ImplGlfw_MouseButtonCallback(window, 0, 1, 0);
		imguiInitialized = true;
		std::cout << "ImGui Initialized..." << "\n";
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	glfwPollEvents();
	RECT rect;
	if (GetWindowRect(hwnd, &rect))
	{
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		io.DisplaySize.x = width;
		io.DisplaySize.y = height;
		//glfwGetFramebufferSize(window, &width, &height);
	}

	ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("FuckTheDevil");

	ImGui::SetWindowPos(ImVec2(0, 100), ImGuiCond_Once);
	ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Once);
	
	//ImGui::CaptureMouseFromApp(true);
	//ImGui::CaptureKeyboardFromApp(true);

	if (ImGui::CollapsingHeader("Stats"))
	{
		ImGui::Text("FPS: %f", io.Framerate);
		ImGui::Text("Gems: %d", cache_gems);
		ImGui::Text("Score: %f", cache_timer);
		ImGui::Text("Enemies: %d", cache_enemiesCounter);
	}
	
	if (ImGui::CollapsingHeader("Settings"))
	{
		ImGui::InputInt("FOV", &player->iFOV, 1);
		if (ImGui::Button("Freecam"))
		{
			triggerFreecam = true;
		}
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

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	GL::RestoreGL();
}


GL::twglSwapBuffers owglSwapBuffers;
BOOL __stdcall hwglSwapBuffers(HDC hDc)
{
	OpenGL_loop();
	return owglSwapBuffers(hDc);
}

//https://guidedhacking.com/threads/opengl-swapbuffers-hook-template-source-code.11476/
void hookOpenGL()
{
	const char *asd = "wglSwapBuffers";
	GL::Hook((char*)asd, (uintptr_t &)owglSwapBuffers, &hwglSwapBuffers);
}

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
	GetClientRect(hwnd, (LPRECT)&windowPos);
	ClientToScreen(hwnd, (LPPOINT)&windowPos.left);
	ClientToScreen(hwnd, (LPPOINT)&windowPos.right);
	windowPos.top -= FindExtraWindowHeight(hwnd);

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

	const auto base = reinterpret_cast< uintptr_t >( GetModuleHandleA( nullptr ) );
	if( !base )
		return 1;
	std::cout << "Game base address: 0x" << std::hex << std::uppercase << base << "\n";

	const auto player_addr = base + 0x1F30C0;
	std::cout << "CPlayer address 0x" << std::hex << std::uppercase << player_addr << "\n\n";

	// dd.exe+4C067 - 01 06 - add [esi],eax
	// dd.exe+4C074 - 8B 01 - mov eax,[ecx]
	// dd.exe+4C084 - 89 01 - mov [ecx],eax
	NOP( (PVOID)( base + 0x4C067 ), 2 );
	NOP( (PVOID)( base + 0x4C074 ), 2 );
	NOP( (PVOID)( base + 0x4C084 ), 2 );

	player = *reinterpret_cast< CPlayer ** >( player_addr );
	
	hookOpenGL();
	hookMouse();

	while( true ) 
	{
		//handle mouse events
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!player)
			continue;
		std::cout << " player->bAlive: " << player->bAlive 
		          << " player->iGems: " << player->iGems
		          << " player->fTimer: " << player->fTimer 
		          << " player->iEnemiesCounter: " << player->iEnemiesCounter 
			<< "\r";

		if (changeLevel)
		{
			changeLevel = false;
			//strcpy(player->sGameMode, textInputBuffer_level);
			strcpy(player->sGameMode, "SECRET");
		}
		else
		{
			strcpy(textInputBuffer_level, player->sGameMode);
		}

		if (triggerFreecam)
		{
			triggerFreecam = false;
			bFreecam = !bFreecam;
			if (bFreecam)
			{
				NOP((PVOID)(base + 0x1A075), 3);
			}
			else
			{
				for (unsigned int i = 0; i < sizeof(originalFreecam) / sizeof(originalFreecam[0]); i = i + 1)
				{
					write((PVOID)(base + 0x1A075 + i), originalFreecam[i], 1);
				}
			}
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

