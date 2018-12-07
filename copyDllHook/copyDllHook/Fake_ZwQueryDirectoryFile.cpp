#include "stdafx.h"
#include "copyDllHook.h"
#include "ntstatus.h"
#include <string>
#include "../../OutGoingFileTool/OutGoingFileTool/FIlestruct.h"
#include <mutex>

NTSTATUS NTAPI Fake_ZwQueryDirectoryFile(
	_In_     HANDLE                 FileHandle,
	_In_opt_ HANDLE                 Event,
	_In_opt_ PIO_APC_ROUTINE        ApcRoutine,
	_In_opt_ PVOID                  ApcContext,
	_Out_    PIO_STATUS_BLOCK       IoStatusBlock,
	_Out_    PVOID                  FileInformation,
	_In_     ULONG                  Length,
	_In_     FILE_INFORMATION_CLASS FileInformationClass,
	_In_     BOOLEAN                ReturnSingleEntry,
	_In_opt_ PUNICODE_STRING        FileName0,
	_In_     BOOLEAN                RestartScan
)
{
	NTSTATUS status;
	IO_STATUS_BLOCK iostatus = { 0 };
	NTSTATUS ntStatus = 0;
	FILE_STANDARD_INFORMATION fsi = { 0 };
	OutputDebugStringEx("Fake_ZwQueryDirectoryFile::获取的文件长度:%d", fsi.EndOfFile.QuadPart);
	status = m_pfnOriginalZwQueryDirectoryFile(FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		FileInformation,
		Length,
		FileInformationClass,
		ReturnSingleEntry,
		FileName0,
		RestartScan);
	if (!NT_SUCCESS(ntStatus))
		   return ntStatus;
	//if (FileInformationClass == FileStandardInformation)
	//{
	//	std::mutex mutexObj;
	//	bool bRet;
	//	int HeadFlaglength = sizeof(RjFileSrtuct) + 1;
	//	bRet = !m_handleList.empty();
	//	if (bRet)
	//	{
	//		mutexObj.lock();
	//		for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
	//		{
	//			if (handleListNode->FileHandle == FileHandle)
	//			{
	//				fsi = *(FILE_STANDARD_INFORMATION*)FileInformation;
	//				//OutputDebugStringEx("Fake_ZwQueryDirectoryFile::获取的文件长度:%d", fsi.EndOfFile.QuadPart);
	//				fsi.EndOfFile.QuadPart -= HeadFlaglength;
	//			}
	//		}
	//		mutexObj.unlock();
	//	}
	//}
	return ntStatus;
}
