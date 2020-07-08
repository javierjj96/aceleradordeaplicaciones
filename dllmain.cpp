
#include "pch.h"
#include "Detours/detours.h"
#include <cstdint>
#include <iostream>
#pragma warning(disable:4996)
#include <mmsystem.h>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment( lib, "Winmm.lib" )
#pragma comment( lib, "Detours/Detours.lib" )
#pragma comment(lib,"ws2_32.lib")

// #include "SpeedHack.h"
// TODO: put in another file, and rename to something better
double vel = 1.0;

bool duplicar = true;

typedef HMODULE(__stdcall* hLoadLibrary)(LPCTSTR lpFileName);
hLoadLibrary oLoadLibrary;




HMODULE WINAPI mLoadLibrary(LPCTSTR lpFileName)
{
	return GetModuleHandle(NULL);
}

template<class DataType>
class SpeedHack {
	DataType time_offset;
	DataType time_last_update;

	double speed_;

public:
	SpeedHack(DataType currentRealTime, double initialSpeed) {
		time_offset = currentRealTime;
		time_last_update = currentRealTime;

		speed_ = initialSpeed;
	}

	// TODO: put lock around for thread safety
	void setSpeed(DataType currentRealTime, double speed) {
		time_offset = getCurrentTime(currentRealTime);
		time_last_update = currentRealTime;

		speed_ = speed;
	}

	// TODO: put lock around for thread safety
	DataType getCurrentTime(DataType currentRealTime) {
		DataType difference = currentRealTime - time_last_update;

		return (DataType)(speed_ * difference) + time_offset;
	}
};

void KeyboardHook()
{
	DWORD GetAsyncAddress = (DWORD)GetProcAddress(LoadLibrary(L"user32.dll"), "GetAsyncKeyState");
	DWORD dwOldProtect;
	VirtualProtect((LPVOID)GetAsyncAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*(BYTE*)(GetAsyncAddress) = 0x8B;
	*(BYTE*)(GetAsyncAddress + 1) = 0xFF;
	*(BYTE*)(GetAsyncAddress + 2) = 0x55;
	*(BYTE*)(GetAsyncAddress + 3) = 0x8B;
	*(BYTE*)(GetAsyncAddress + 4) = 0xEC;
	VirtualProtect((LPVOID)GetAsyncAddress, 4, dwOldProtect, NULL);
}
// function signature typedefs
typedef DWORD(WINAPI* GetTickCountType)(void);
typedef ULONGLONG(WINAPI* GetTickCount64Type)(void);

typedef BOOL(WINAPI* QueryPerformanceCounterType)(LARGE_INTEGER* lpPerformanceCount);

// globals
GetTickCountType   g_GetTickCountOriginal = (GetTickCountType)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "GetTickCount");
GetTickCount64Type g_GetTickCount64Original = (GetTickCount64Type)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "GetTickCount64");
GetTickCountType   g_TimeGetTimeOriginal = (GetTickCountType)GetProcAddress(GetModuleHandleA("Winmm.dll"), "timeGetTime");

QueryPerformanceCounterType g_QueryPerformanceCounterOriginal;


const double kInitialSpeed = 1.0; // initial speed hack speed

//                                  (initialTime,      initialSpeed)
SpeedHack<DWORD>     g_speedHack(GetTickCount(), kInitialSpeed);
SpeedHack<ULONGLONG> g_speedHackULL(GetTickCount64(), kInitialSpeed);
SpeedHack<LONGLONG>  g_speedHackLL(0, kInitialSpeed); // Gets set properly in DllMain

// function prototypes

DWORD     WINAPI GetTickCountHacked(void);
ULONGLONG WINAPI GetTickCount64Hacked(void);

BOOL      WINAPI QueryPerformanceCounterHacked(LARGE_INTEGER* lpPerformanceCount);

DWORD     WINAPI KeysThread(LPVOID lpThreadParameter);

// functions

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved) {
	// TODO: split up this function for readability.

	if (dwReason == DLL_PROCESS_ATTACH) {
		
		HINSTANCE hGetProcIDDLL = LoadLibrary(L"C:\\Mgame\\Ares\\Ares_Language2.dll");

		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		std::cout << "This works" << std::endl;

		HMODULE kernel32 = GetModuleHandle(L"Kernel32.dll");
		HMODULE winmm = GetModuleHandle(L"Winmm.dll");

		// TODO: check if the modules are even loaded.

		// Get all the original addresses of target functions
		g_GetTickCountOriginal = (GetTickCountType)GetProcAddress(kernel32, "GetTickCount");
		g_GetTickCount64Original = (GetTickCount64Type)GetProcAddress(kernel32, "GetTickCount64");

		g_TimeGetTimeOriginal = (GetTickCountType)GetProcAddress(winmm, "timeGetTime");

		g_QueryPerformanceCounterOriginal = (QueryPerformanceCounterType)GetProcAddress(kernel32, "QueryPerformanceCounter");

		// Setup the speed hack object for the Performance Counter
		LARGE_INTEGER performanceCounter;
		g_QueryPerformanceCounterOriginal(&performanceCounter);

		//g_speedHackLL = SpeedHack<LONGLONG>(performanceCounter.QuadPart, kInitialSpeed);

		// Detour functions
		DetourTransactionBegin();

		DetourAttach((PVOID*)&g_GetTickCountOriginal, (PVOID)GetTickCountHacked);
		DetourAttach((PVOID*)&g_GetTickCount64Original, (PVOID)GetTickCount64Hacked);
		//DetourAttach((PVOID*)LoadLibrary, (PVOID)mLoadLibrary);
		
		//DetourAttach(&(PVOID&)pSend, MySend);
		// Detour timeGetTime to the hacked GetTickCount (same signature)
		DetourAttach((PVOID*)&g_TimeGetTimeOriginal, (PVOID)GetTickCountHacked);

		

		DetourTransactionCommit();

		CreateThread(NULL, 0, KeysThread, NULL, 0, NULL);
	}

	return TRUE;
}

void setAllToSpeed(double speed) {
	g_speedHack.setSpeed(g_GetTickCountOriginal(), speed);

	g_speedHackULL.setSpeed(g_GetTickCount64Original(), speed);

	LARGE_INTEGER performanceCounter;
	g_QueryPerformanceCounterOriginal(&performanceCounter);

	g_speedHackLL.setSpeed(performanceCounter.QuadPart, speed);
}

void ayuda(double vel)
{
	setAllToSpeed(vel);
	system("cls");
	printf("Velocidad %.1f\n", vel);
}
DWORD WINAPI KeysThread(LPVOID lpThreadParameter) {

	while (true) {
		if (GetAsyncKeyState(VK_XBUTTON1) & 1) {
			vel = 1.0;
			ayuda(vel);
			duplicar = true;
		}
		if (GetAsyncKeyState(VK_XBUTTON2) & 1) {
			vel = vel + 0.5;
			ayuda(vel);
			duplicar = false;
		}
		if (GetAsyncKeyState(VK_END) & 1) {
			vel = 15.0;
			ayuda(vel);	
		}
		if (GetAsyncKeyState(VK_SPACE) & 1) {
			vel = 15.0;
			ayuda(vel);			
			for (size_t i = 0; i < 50; i++)
			{
				Sleep(10);
			}
			vel = 1.0;
			ayuda(vel);	
		}
		KeyboardHook();
		Sleep(10);
	}
	return 0;
}


DWORD WINAPI GetTickCountHacked(void) {
	return g_speedHack.getCurrentTime(g_GetTickCountOriginal());
}

ULONGLONG WINAPI GetTickCount64Hacked(void) {
	return g_speedHackULL.getCurrentTime(g_GetTickCount64Original());
}

BOOL WINAPI QueryPerformanceCounterHacked(LARGE_INTEGER* lpPerformanceCount) {
	LARGE_INTEGER performanceCounter;

	BOOL result = g_QueryPerformanceCounterOriginal(&performanceCounter);

	lpPerformanceCount->QuadPart = g_speedHackLL.getCurrentTime(performanceCounter.QuadPart);

	return result;
}