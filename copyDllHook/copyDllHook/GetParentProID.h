#ifndef __GETPARENTPROID_H__
#define __GETPARENTPROID_H__

#include "stdafx.h"

#include <stdio.h>

#define ProcessBasicInformation 0
typedef struct
{
	DWORD ExitStatus;
	DWORD PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
}   PROCESS_BASIC_INFORMATION;

typedef LONG(WINAPI *PROCNTQSIP)(HANDLE, UINT, PVOID, ULONG, PULONG);
PROCNTQSIP NtQueryInformationProcess;

DWORD GetParentProcessID(DWORD dwId)
{
	LONG                      status;
	DWORD                     dwParentPID = (DWORD)-1;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;

	NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryInformationProcess");
	if (!NtQueryInformationProcess)
		return -1;

	// Get process handle
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwId);
	if (!hProcess)
		return (DWORD)-1;

	// Retrieve information
	status = NtQueryInformationProcess(hProcess,
		ProcessBasicInformation,
		(PVOID)&pbi,
		sizeof(PROCESS_BASIC_INFORMATION),
		NULL
		);

	// Copy parent Id on success
	if (!status)
		dwParentPID = pbi.InheritedFromUniqueProcessId;

	CloseHandle(hProcess);

	return dwParentPID;
}

//创建一个线程监听父进程是否关闭

unsigned int __stdcall ThreadFun(DWORD dwChildID)
{
	HANDLE                    hProcess;
	while (!(hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwChildID)))
	{
		CloseHandle(hProcess);
		Sleep(1000);
		continue;
	}
	
	TerminateProcess(GetCurrentProcess(), 0);
	return 0;
}
void listenParentProcess(DWORD dwChildID)
{
	HANDLE handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFun, (LPVOID)dwChildID, 0, NULL);
	//WaitForSingleObject(handle, INFINITE);
}


#endif//__GETPARENTPROID_H__