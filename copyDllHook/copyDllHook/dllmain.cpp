// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "copyDllHook.h"
#include "GlobalHook.h"
bool bhook=false;

BOOL	 IsOrigProcess(CHAR* pExt)
{
	CHAR			p1[] = "et.exe";
	CHAR			p2[] = "wpsoffice.exe";
	CHAR			p3[] = "wpp.exe";
	CHAR			p4[] = "wps.exe";
	CHAR			p5[] = "WINWORD.EXE";
	CHAR			p6[] = "POWERONT.EXE";
	CHAR			p7[] = "EXECEL.EXE";
	return (strstr(pExt, p1) == 0 || strstr(pExt, p2) == 0 || strstr(pExt, p3) == 0 || strstr(pExt, p4) == 0 || strstr(pExt, p5) == 0 || strstr(pExt, p6) == 0 || strstr(pExt, p7) == 0);
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
	    if (IsOrigProcess(cIniFileName)==NULL)
	    {
            OutputDebugStringEx("Discovery process!\r\n");
            return 1;
	    }
		OutputDebugStringEx("Is a process!\r\n");
	    bhook = true;
		StartHook();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (bhook)
		{
			EndHook();
		}
		break;
	}
	return TRUE;
}

