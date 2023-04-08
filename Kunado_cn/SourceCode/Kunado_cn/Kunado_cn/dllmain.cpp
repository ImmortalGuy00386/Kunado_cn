#include <Windows.h>
#include "../TDA/MemX.h"
#include "../TDA/DetoursX.h"
#include "../CmvsFileHook/CmvsFileHook.h"


static DWORD g_dwExeBase = (DWORD)GetModuleHandleW(NULL);

VOID StartHook()
{
	TDA::DetoursX::HookCreateFontA(0x86, "黑体");

	SetPS3Hook_380_(g_dwExeBase + 0x6CE10, g_dwExeBase + 0x6CEF1, g_dwExeBase + 0x6CF1D);
	SetPB3Hook_380_(g_dwExeBase + 0x3FE8D);

	BYTE aRange[] = {0xFE};
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0x18379), aRange, sizeof(aRange));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0x19DB9), aRange, sizeof(aRange));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0x1A0CB), aRange, sizeof(aRange));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0x1A109), aRange, sizeof(aRange));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0x1A0CB), aRange, sizeof(aRange));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0x1A156), aRange, sizeof(aRange));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0x1A1C6), aRange, sizeof(aRange));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0x1A239), aRange, sizeof(aRange));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0x1A34B), aRange, sizeof(aRange));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0x1A429), aRange, sizeof(aRange));

	BYTE patchFont[] = "黑体";
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0xD0C08), patchFont, sizeof(patchFont));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0xE9298), patchFont, sizeof(patchFont));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0xE9670), patchFont, sizeof(patchFont));
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0xE97B0), patchFont, sizeof(patchFont));

	BYTE aTitleFix[] = { 0xA1,0xA1 }; 
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0xD1DD4), (LPVOID)aTitleFix, sizeof(aTitleFix));

	BYTE aBoot[] = "启动设置";
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0xD2168), aBoot, sizeof(aBoot));

	BYTE aStan[] = "[标准]";
	TDA::MemX::WriteMemory((LPVOID)(g_dwExeBase + 0xD218C), aStan, sizeof(aStan));
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		StartHook();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" VOID __declspec(dllexport) Dir_A() {}