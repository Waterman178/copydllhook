#include "stdafx.h"
#include "GlobalHook.h"
#include "DebugPrintf.h"

HHOOK g_HookKey;
HHOOK g_HookMsg;

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>键盘钩子的回调函数<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

LRESULT CALLBACK FunKey(
	int code, // hook code
	WPARAM wParam, // virtual-key code
	LPARAM lParam // keystroke-message information
	)
{
	//OutputDebugStringEx("测试3 CALLBACK FUNCTION....\n");
	KBDLLHOOKSTRUCT *Key_Info = (KBDLLHOOKSTRUCT*)lParam;
	//OutputDebugStringEx("测试3 code: %d, wParam=%d, lParam=%d\n", code, wParam, lParam);

	if (HC_ACTION == code)
	{

		//OutputDebugStringEx("测试3 Key_Info: %d\n", Key_Info->vkCode);
		if (WM_KEYDOWN == wParam || WM_SYSKEYDOWN == wParam)
		{
			if (Key_Info->vkCode == VK_SNAPSHOT)//屏蔽 PrintScreen 按键
			{
				return TRUE;
			}		
		}

	}
	return CallNextHookEx(g_HookKey, code, wParam, lParam);
}
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>信息钩子的回调函数<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
LRESULT CALLBACK MyMessageProc(int code, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(g_HookMsg, code, wParam, lParam);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>DLL导出函数集合<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
void __stdcall StartHookKeyBord() //安装键盘钩子
{
	g_HookKey = SetWindowsHookEx(WH_KEYBOARD_LL, FunKey, GetModuleHandle(TEXT("copyDllHook.dll")),0);
}

BOOL __stdcall EndHookKeyBord()//卸载键盘钩子 
{ 
	if (UnhookWindowsHookEx(g_HookKey) == 0) return FALSE;
	g_HookKey = NULL;
	return TRUE;
}

void __stdcall StartHookMsg()
{
#ifdef x64
	g_HookMsg = SetWindowsHookEx(WH_CALLWNDPROCRET, MyMessageProc, GetModuleHandle(TEXT("copyDllHookx64.dll")), 0);
#else
	g_HookMsg = SetWindowsHookEx(WH_CALLWNDPROCRET, MyMessageProc, GetModuleHandle(TEXT("copyDllHookx86.dll")), 0);
#endif
}
BOOL __stdcall EndHookMsg()
{
	if (UnhookWindowsHookEx(g_HookMsg) == 0) return FALSE;
	g_HookMsg = NULL;
	return TRUE;
}
int InjectDll(DWORD dwProcessId, PTCHAR szDllName)
{
	if (szDllName[0] == NULL)
		return -1;
	//提高权限相关操作  
	//EnablePrivilege(TRUE);
	//1. 打开进程  
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,   //打开进程权限  
		FALSE,                                              //是否可继承   
		dwProcessId);                                       //进程ID  

	if (hProcess == INVALID_HANDLE_VALUE)
		return -1;

	//2. 在远程进程中申请空间  
	LPVOID pszDllName = ::VirtualAllocEx(hProcess, //远程进程句柄  
		NULL,                                  //建议开始地址  
		4096,                                  //分配空间大小  
		MEM_COMMIT,                            //空间初始化全0  
		PAGE_EXECUTE_READWRITE);               //空间权限  

	if (NULL == pszDllName)
	{
		return -1;
	}

	//3. 向远程进程中写入数据  
	BOOL bRet = ::WriteProcessMemory(hProcess, pszDllName,
		szDllName, MAX_PATH, NULL);

	if (NULL == bRet)
	{
		return -1;
	}

	//4. 在远程进程中创建远程线程 
	HANDLE m_hInjecthread = NULL;
	m_hInjecthread = ::CreateRemoteThread(hProcess,      //远程进程句柄  
		NULL,                                            //安全属性  
		0,                                               //栈大小  
		(LPTHREAD_START_ROUTINE)LoadLibrary,             //进程处理函数      
		pszDllName,                                      //传入参数  
		NULL,                                            //默认创建后的状态  
		NULL);                                           //线程ID  

	if (NULL == m_hInjecthread)
	{
		DWORD dwErr = GetLastError();
		return -1;
	}

	//5. 等待线程结束返回  
	DWORD dw = WaitForSingleObject(m_hInjecthread, -1);
	//6. 获取线程退出码,即LoadLibrary的返回值，即dll的首地址  
	DWORD dwExitCode;
	HMODULE m_hMod;
	GetExitCodeThread(m_hInjecthread, &dwExitCode);
	m_hMod = (HMODULE)dwExitCode;

	//7. 释放空间  
	BOOL bReturn = VirtualFreeEx(hProcess, pszDllName,
		4096, MEM_DECOMMIT);

	if (NULL == bReturn)
	{
		return -1;
	}

	CloseHandle(hProcess);
	hProcess = NULL;
	//恢复权限相关操作  
	//EnablePrivilege(FALSE);

	return 0;
}