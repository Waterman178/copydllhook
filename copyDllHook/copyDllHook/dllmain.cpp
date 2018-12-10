// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "copyDllHook.h"
#include "GlobalHook.h"
bool bhook=false;
HMODULE g_hDll = NULL;

HANDLE hMutex = NULL;
DWORD WINAPI UnloadProc(PVOID param)
{
	MessageBox(NULL, TEXT("Press ok to unload me."),
		TEXT("MsgBox in dll"), MB_OK);
	FreeLibrary(g_hDll);
	// oops!  
	return 0;
}


BOOL	 IsOrigProcess(CHAR* pExt)
{
	CHAR			p1[] = "et.exe";
	CHAR			p2[] = "wpsoffice.exe";
	CHAR			p3[] = "wpp.exe";
	CHAR			p4[] = "wps.exe";
	CHAR			p5[] = "WINWORD.EXE";
	CHAR			p6[] = "POWERONT.EXE";
	CHAR			p7[] = "NOTEPAD.EXE";
	CHAR			p8[] = "EXCEL.EXE";
	return (strstr(pExt, p1) != NULL  || strstr(pExt, p2) != NULL || strstr(pExt, p3) != NULL || strstr(pExt, p4) != NULL || strstr(pExt, p5) != NULL || strstr(pExt, p6) != NULL || strstr(pExt, p7) != NULL || strstr(pExt, p8) != NULL);
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	char cIniFileName[MAX_PATH];
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetModuleFileNameA(NULL, cIniFileName, sizeof(cIniFileName));
		//OutputDebugStringEx("cIniFileName%s，PID:%d \r\n", cIniFileName, GetCurrentProcessId());
		if (IsOrigProcess(cIniFileName) == NULL)
		{
			break;
		}
	    bhook = true;
		StartHook();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugStringEx("DLL EXIT!\r\n");
		if (bhook)
		{
			EndHook();
		}
		break;
	}
	return TRUE;
}

