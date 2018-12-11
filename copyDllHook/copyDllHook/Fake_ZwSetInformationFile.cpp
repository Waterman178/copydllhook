#include "stdafx.h"
#include "copyDllHook.h"
#include "ntstatus.h"
#include <string>
#include "../../OutGoingFileTool/OutGoingFileTool/FIlestruct.h"
#include <mutex> 
typedef struct _FILE_END_OF_FILE_INFORMATION {
	LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION;

typedef struct _FILE_ALLOCATION_INFORMATION {
	LARGE_INTEGER AllocationSize;
} FILE_ALLOCATION_INFORMATION, *PFILE_ALLOCATION_INFORMATION;

typedef struct _FILE_INTERNAL_INFORMATION {
	LARGE_INTEGER IndexNumber;
} FILE_INTERNAL_INFORMATION, *PFILE_INTERNAL_INFORMATION;

typedef struct _FILE_EA_INFORMATION {
	ULONG EaSize;
} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

typedef struct _FILE_ACCESS_INFORMATION {
	ACCESS_MASK AccessFlags;
} FILE_ACCESS_INFORMATION, *PFILE_ACCESS_INFORMATION;

typedef struct _FILE_DIRECTORY_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	WCHAR         FileName666[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef struct _FILE_FULL_DIR_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	ULONG         EaSize;
	WCHAR         FileName77[1];
} FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;



typedef struct _FILE_MODE_INFORMATION {
	ULONG Mode;
} FILE_MODE_INFORMATION, *PFILE_MODE_INFORMATION;

typedef struct _FILE_ALIGNMENT_INFORMATION {
	ULONG AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION;

typedef struct _FILE_NAME_INFORMATION {
	ULONG FileNameLength;
	WCHAR FileNames[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

typedef struct _FILE_ALL_INFORMATION {
	FILE_BASIC_INFORMATION     BasicInformation;
	FILE_STANDARD_INFORMATION  StandardInformation;
	FILE_INTERNAL_INFORMATION  InternalInformation;
	FILE_EA_INFORMATION        EaInformation;
	FILE_ACCESS_INFORMATION    AccessInformation;
	FILE_POSITION_INFORMATION  PositionInformation;
	FILE_MODE_INFORMATION      ModeInformation;
	FILE_ALIGNMENT_INFORMATION AlignmentInformation;
	FILE_NAME_INFORMATION      NameInformation;
} FILE_ALL_INFORMATION, *PFILE_ALL_INFORMATION;

typedef struct _FILE_BOTH_DIR_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	ULONG         EaSize;
	CCHAR         ShortNameLength;
	WCHAR         ShortName[12];
	WCHAR         FileName33[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

typedef struct _FILE_ID_FULL_DIR_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	ULONG         EaSize;
	LARGE_INTEGER FileId;
	WCHAR         FileNam2e1[1];
} FILE_ID_FULL_DIR_INFORMATION, *PFILE_ID_FULL_DIR_INFORMATION;
typedef struct _FILE_ID_GLOBAL_TX_DIR_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	LARGE_INTEGER FileId;
	GUID          LockingTransactionId;
	ULONG         TxInfoFlags;
	WCHAR         FileName22[1];
} FILE_ID_GLOBAL_TX_DIR_INFORMATION, *PFILE_ID_GLOBAL_TX_DIR_INFORMATION;
typedef struct _FILE_ID_BOTH_DIR_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	ULONG         EaSize;
	CCHAR         ShortNameLength;
	WCHAR         ShortName[12];
	LARGE_INTEGER FileId;
	WCHAR         FileNam335e[1];
} FILE_ID_BOTH_DIR_INFORMATION, *PFILE_ID_BOTH_DIR_INFORMATION;



NTSTATUS WINAPI HookSetInformathionFile(HANDLE  FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID  FileInformation, ULONG  Length, FILE_INFORMATION_CLASS FileInformationClass) {
	NTSTATUS ntStatus = STATUS_SUCCESS;
	std::mutex mutexObj;
	FileHandleRelationNode* pRobj = NULL;
	FILE_STANDARD_INFORMATION fpi;
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
		mutexObj.lock();
		for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
		{
			if (handleListNode->FileHandle == FileHandle)
			{
				pRobj->FileHandle = handleListNode->FileHandle;
				pRobj->m_FileInfo = handleListNode->m_FileInfo;
				ntStatus = m_pfnOriginalZwQueryInformationFile(FileHandle,
					&iostatus,
					&fpi,    // current pos
					sizeof(FILE_STANDARD_INFORMATION),
					FileStandardInformation);
				if (FileInformationClass == FileEndOfFileInformation)
				{
				((FILE_END_OF_FILE_INFORMATION*)FileInformation)->EndOfFile.QuadPart += HeaderLength;
				OutputDebugStringEx("HookSetInformathionFile::FileEndOfFileInformation!!!!!!!");
				}
				if (FileInformationClass == FileAllocationInformation)
				{
				((FILE_ALLOCATION_INFORMATION*)FileInformation)->AllocationSize.QuadPart += HeaderLength;
				OutputDebugStringEx("HookSetInformathionFile::FileAllocationInformation!!!!!!!");
				}
				if (FileInformationClass == FilePositionInformation)
				{
				((PFILE_POSITION_INFORMATION)FileInformation)->CurrentByteOffset.QuadPart += HeaderLength;
				if( ((PFILE_POSITION_INFORMATION)FileInformation)->CurrentByteOffset.QuadPart > fpi.EndOfFile.QuadPart)
				{
				((PFILE_POSITION_INFORMATION)FileInformation)->CurrentByteOffset.QuadPart = fpi.EndOfFile.QuadPart;
				}
				OutputDebugStringEx("HookSetInformathionFile::FilePositionInformation:%d", ((PFILE_POSITION_INFORMATION)FileInformation)->CurrentByteOffset.QuadPart);
				}
			}
			else
			{
			}
		}
		mutexObj.unlock();
		delete pRobj;
		return m_pfnOriginalZwSetInformationFile(FileHandle, IoStatusBlock, FileInformation, Length, FileInformationClass);
	}
	return m_pfnOriginalZwSetInformationFile(FileHandle, IoStatusBlock, FileInformation, Length, FileInformationClass);
}