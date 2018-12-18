#include <stdio.h>
#include <windows.h>
#define   WM_UPDATE_STATIC  (WM_USER + 0x100)
#define   WM_EXITPROCESS   (WM_USER + 0x103)
void(WINAPI *InstallHook)();
void(WINAPI *UnstallHook)();
PVOID LoadDllFunc(LPCTSTR lpFileName, LPCSTR lpProcName){
	PVOID insthook;
	HINSTANCE hinst = NULL;
	hinst = LoadLibrary(lpFileName);
	if (!hinst)
		return NULL;
	insthook = GetProcAddress(hinst, lpProcName);
	if (!insthook)
		return NULL;
	return insthook;
}
BOOLEAN init() {
	InstallHook = (void (WINAPI*)())LoadDllFunc(TEXT("copyDllHookx64.dll"), "StartHookMsg");
	UnstallHook = (void(WINAPI*)())LoadDllFunc(TEXT("copyDllHookx64.dll"), "EndHookMsg");
	if (InstallHook != NULL && UnstallHook != NULL)
		return TRUE;
	return FALSE;
}
void main() {
	MSG msg = { 0 };
	BOOLEAN bRet;
	char ConsoleTitle[1024] = {0};
	if (init())
		InstallHook();
	else
		return;
	SetConsoleTitleA("Wrenchx64");
	GetConsoleTitleA(ConsoleTitle, 1024);
	HWND PWND = FindWindowA(NULL, ConsoleTitle);
	printf_s("ConsoleTitle:%s PWND:%08x \n", ConsoleTitle, PWND);
	while (GetMessageA(&msg, NULL, 0, WM_USER + 0x103))
	{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_EXITPROCESS)
			{
				if (msg.lParam == 0 && msg.wParam == 0)
				{
					printf_s("process exit,UnstallHooking...");
					UnstallHook();
					printf_s("okey,bye~");
					exit(0);
				}
			}
	}
}