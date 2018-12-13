#include "stdafx.h"
#include "copyDllHook.h"
#include "ntstatus.h"
#include <string>
#include "../../OutGoingFileTool/OutGoingFileTool/FIlestruct.h"
#include <mutex> 
 NTSTATUS WINAPI HookZwWriteFile(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
) {
	NTSTATUS ntStatus = STATUS_SUCCESS;
	std::mutex mutexObj;
	FileHandleRelationNode* pRobj = NULL;
	FILE_POSITION_INFORMATION fpi;
	IO_STATUS_BLOCK iostatus;
	LARGE_INTEGER lOldOffset = { 0 };
	LARGE_INTEGER lCurrentOffset = { 0 };
	bool bRet = FALSE;
	BOOL bOverRideRet = FALSE;
	OVERLAPPED pOverlapped;
	DWORD dwReaded = 0;
	auto HeaderLength = sizeof(RjFileSrtuct) + 1;
	bRet = !m_handleList.empty();
	if (bRet)
	{
		pRobj = new FileHandleRelationNode;
		pRobj->m_FileInfo.bReadDecrypt = FALSE;
		mutexObj.lock();
		for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
		{
			if (handleListNode->FileHandle == FileHandle)
			{
				pRobj->FileHandle = handleListNode->FileHandle;
				pRobj->m_FileInfo = handleListNode->m_FileInfo;
				OutputDebugStringEx("HookZwWriteFile!Find!!!!!!!");
				for (int i = 0; i < Length; i++)
				{
					reinterpret_cast<char*>(Buffer)[i] ^= 'a';
				}
				return m_pfnOriginalZwWriteFile(
					FileHandle,
					Event,
					ApcRoutine,
					ApcContext,
					IoStatusBlock,
					(char*)Buffer,
					Length,
					ByteOffset+HeaderLength,
					Key
				);
			}
			else
			{
				pRobj->m_FileInfo.bReadDecrypt = FALSE;
				bRet = FALSE;
			}
		}
		mutexObj.unlock();
		delete pRobj;
	}
	return m_pfnOriginalZwWriteFile(
		FileHandle,
		Event,
	    ApcRoutine,
	    ApcContext,
	    IoStatusBlock,
	    Buffer,
		Length,
	    ByteOffset,
		Key
	);
 }