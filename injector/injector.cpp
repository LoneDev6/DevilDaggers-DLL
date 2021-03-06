#include "pch.h"
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <thread>
#include <future>
#include <filesystem>
#include <fstream>
#include <vector>

void inject_dll(LPCSTR dll_path, __int64 game_pid);
DWORD FindProcessId(const std::wstring& processName);
std::string get_exe_filename();
std::string get_working_dir();

int main()
{
	std::cout << "Loading settings..." << std::endl;

	std::string working_dir = get_working_dir();

	std::ifstream settings_file(get_working_dir() + "/settings.txt");
	if (!settings_file)
	{
		std::cout << "Error opening settings.txt file." << std::endl;
		system("pause");
		return -1;
	}

	int i = -1;
	std::wstring game_name;
	std::vector<std::string> dlls;
	while (!settings_file.eof())
	{
		i++;
		std::string tmp;
		if (i == 0 || i == 2)//skip desc lines
		{
			std::getline(settings_file, tmp);
			continue;
		}
		if (i == 1) //exe name
		{
			std::getline(settings_file, tmp);
			game_name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(tmp);
			continue;
		}
		std::getline(settings_file, tmp);
		dlls.push_back(tmp);
	}

	std::cout << "Waiting game..." << std::endl;
	DWORD game_pid;
	while (true)
	{
		game_pid = FindProcessId(game_name);
		if (game_pid != -1)
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	for (auto &element : dlls)
		inject_dll((working_dir + "\\\\" + element).c_str(), game_pid);

	system("pause");
}

void inject_dll(LPCSTR dll_path, __int64 game_pid)
{
	HANDLE handle_target = OpenProcess(PROCESS_ALL_ACCESS, FALSE, game_pid);

	LPVOID pDllPath = VirtualAllocEx(handle_target, 0, strlen(dll_path) + 1, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(handle_target, pDllPath, (LPVOID)dll_path, strlen(dll_path) + 1, 0);

	LPTHREAD_START_ROUTINE _LoadLibraryA = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");
	HANDLE hLoadThread = CreateRemoteThread(handle_target, 0, 0, _LoadLibraryA, pDllPath, 0, 0);

	WaitForSingleObject(hLoadThread, INFINITE);

	std::cout << "DLL Injected: " << std::hex << pDllPath << std::endl;

	VirtualFreeEx(handle_target, pDllPath, strlen(dll_path) + 1, MEM_RELEASE);
}


DWORD FindProcessId(const std::wstring& process_name)
{
	PROCESSENTRY32 pe = { sizeof(pe) };

	auto processes_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0);
	if (processes_snapshot == INVALID_HANDLE_VALUE)
		return -1;

	Process32First(processes_snapshot, &pe);
	if (!process_name.compare(pe.szExeFile))
	{
		CloseHandle(processes_snapshot);
		return pe.th32ProcessID;
	}

	while (Process32Next(processes_snapshot, &pe))
	{
		if (!process_name.compare(pe.szExeFile))
		{
			CloseHandle(processes_snapshot);
			return pe.th32ProcessID;
		}
	}

	CloseHandle(processes_snapshot);
	return -1;
}

std::string get_exe_filename()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}

std::string get_working_dir()
{
	std::string f = get_exe_filename();
	return f.substr(0, f.find_last_of("\\/"));
}