#include "stdafx.h"
#include "GlobalHook.h"
#include "DebugPrintf.h"

HHOOK g_HookKey;
HHOOK g_HookMsg;

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>���̹��ӵĻص�����<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

LRESULT CALLBACK FunKey(
	int code, // hook code
	WPARAM wParam, // virtual-key code
	LPARAM lParam // keystroke-message information
	)
{
	//OutputDebugStringEx("����3 CALLBACK FUNCTION....\n");
	KBDLLHOOKSTRUCT *Key_Info = (KBDLLHOOKSTRUCT*)lParam;
	//OutputDebugStringEx("����3 code: %d, wParam=%d, lParam=%d\n", code, wParam, lParam);

	if (HC_ACTION == code)
	{

		//OutputDebugStringEx("����3 Key_Info: %d\n", Key_Info->vkCode);
		if (WM_KEYDOWN == wParam || WM_SYSKEYDOWN == wParam)
		{
			if (Key_Info->vkCode == VK_SNAPSHOT)//���� PrintScreen ����
			{
				return TRUE;
			}		
		}

	}
	return CallNextHookEx(g_HookKey, code, wParam, lParam);
}
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>��Ϣ���ӵĻص�����<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
LRESULT CALLBACK MyMessageProc(int code, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(g_HookMsg, code, wParam, lParam);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>DLL������������<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
void __stdcall StartHookKeyBord() //��װ���̹���
{
	g_HookKey = SetWindowsHookEx(WH_KEYBOARD_LL, FunKey, GetModuleHandle(TEXT("copyDllHook.dll")),0);
}

BOOL __stdcall EndHookKeyBord()//ж�ؼ��̹��� 
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
	//���Ȩ����ز���  
	//EnablePrivilege(TRUE);
	//1. �򿪽���  
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,   //�򿪽���Ȩ��  
		FALSE,                                              //�Ƿ�ɼ̳�   
		dwProcessId);                                       //����ID  

	if (hProcess == INVALID_HANDLE_VALUE)
		return -1;

	//2. ��Զ�̽���������ռ�  
	LPVOID pszDllName = ::VirtualAllocEx(hProcess, //Զ�̽��̾��  
		NULL,                                  //���鿪ʼ��ַ  
		4096,                                  //����ռ��С  
		MEM_COMMIT,                            //�ռ��ʼ��ȫ0  
		PAGE_EXECUTE_READWRITE);               //�ռ�Ȩ��  

	if (NULL == pszDllName)
	{
		return -1;
	}

	//3. ��Զ�̽�����д������  
	BOOL bRet = ::WriteProcessMemory(hProcess, pszDllName,
		szDllName, MAX_PATH, NULL);

	if (NULL == bRet)
	{
		return -1;
	}

	//4. ��Զ�̽����д���Զ���߳� 
	HANDLE m_hInjecthread = NULL;
	m_hInjecthread = ::CreateRemoteThread(hProcess,      //Զ�̽��̾��  
		NULL,                                            //��ȫ����  
		0,                                               //ջ��С  
		(LPTHREAD_START_ROUTINE)LoadLibrary,             //���̴�����      
		pszDllName,                                      //�������  
		NULL,                                            //Ĭ�ϴ������״̬  
		NULL);                                           //�߳�ID  

	if (NULL == m_hInjecthread)
	{
		DWORD dwErr = GetLastError();
		return -1;
	}

	//5. �ȴ��߳̽�������  
	DWORD dw = WaitForSingleObject(m_hInjecthread, -1);
	//6. ��ȡ�߳��˳���,��LoadLibrary�ķ���ֵ����dll���׵�ַ  
	DWORD dwExitCode;
	HMODULE m_hMod;
	GetExitCodeThread(m_hInjecthread, &dwExitCode);
	m_hMod = (HMODULE)dwExitCode;

	//7. �ͷſռ�  
	BOOL bReturn = VirtualFreeEx(hProcess, pszDllName,
		4096, MEM_DECOMMIT);

	if (NULL == bReturn)
	{
		return -1;
	}

	CloseHandle(hProcess);
	hProcess = NULL;
	//�ָ�Ȩ����ز���  
	//EnablePrivilege(FALSE);

	return 0;
}