#include "CmvsFileHook.h"
#include "../TDA/MemX.h"

static DWORD g_dwPS3Raw = NULL;
static DWORD g_dwPB3Raw = NULL;
static PBYTE g_dwPS3Dec = NULL;
static PBYTE g_dwPS3Com = NULL;

static PCHAR g_pFileName = 0;
static CHAR g_aHookPath[MAX_PATH] = { 0 };
static CHAR g_aHookFolder[] = ".\\FileHook\\";

BOOL FileExist()
{
	lstrcpyA(g_aHookPath, g_aHookFolder); lstrcatA(g_aHookPath, g_pFileName);

	DWORD isFileExist = GetFileAttributesA(g_aHookPath);
	if ((isFileExist != INVALID_FILE_ATTRIBUTES) && (isFileExist != FILE_ATTRIBUTE_DIRECTORY)) return TRUE;

	return FALSE;
}

VOID __declspec(naked)CMVS_380_PS3Hook()
{
	__asm
	{
		pushfd;
		pushad;
		mov eax, dword ptr[esp + 0x10];
		mov g_pFileName, eax;
	}

	if (FileExist())
	{
		__asm mov dword ptr[esp + 0x18], offset g_aHookFolder;
		*g_dwPS3Dec = 0xEB;
		*g_dwPS3Com = 0xEB;
	}
	else
	{
		*g_dwPS3Dec = 0x74;
		*g_dwPS3Com = 0x74;
	}

	__asm
	{
		popad;
		popfd;
		ret;
	}
}

VOID __declspec(naked)CMVS_380_PB3Hook()
{
	__asm
	{
		pushfd;
		pushad;
		mov eax, dword ptr[esp + 0x2C];
		mov g_pFileName, eax;
	}

	if (FileExist())
	{
		__asm mov dword ptr[esp + 0x4], offset g_aHookFolder;
	}

	__asm
	{
		popad;
		popfd;
		ret;
	}
}

VOID SetPS3Hook_380_(DWORD dwPS3Raw, DWORD dwPS3Dec, DWORD dwPS3Com)
{
	g_dwPB3Raw = dwPS3Raw;
	g_dwPS3Dec = (PBYTE)dwPS3Dec;
	g_dwPS3Com = (PBYTE)dwPS3Com;

	TDA::MemX::SetHook(dwPS3Raw, (DWORD)CMVS_380_PS3Hook, 0x9);
}

VOID SetPB3Hook_380_(DWORD dwPB3Raw)
{
	TDA::MemX::SetHook(dwPB3Raw, (DWORD)CMVS_380_PB3Hook, 0x6);
}