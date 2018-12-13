#include "stdafx.h"
#include "copyDllHook.h"
#include "ntstatus.h"
#include <string>
#include "../../OutGoingFileTool/OutGoingFileTool/FIlestruct.h"
#include <mutex>

//与 SETINFOFILE处理相反 要尽量暴露自己设置的偏移给应用程序
typedef struct _FILE_END_OF_FILE_INFORMATION {
	LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION;

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



FILE_ALL_INFORMATION fsiall;


NTSTATUS NTAPI Fake_ZwQueryInformationFile(HANDLE FileHandle,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	FILE_INFORMATION_CLASS FileInformationClass)
{
	NTSTATUS status = 0;
	IO_STATUS_BLOCK iostatus = { 0 };
	NTSTATUS ntStatus = 0;
	FILE_STANDARD_INFORMATION fsi = { 0 };
	std::mutex mutexObj;
	bool bRet;
	int HeadFlaglength = sizeof(RjFileSrtuct) + 1;
	status = m_pfnOriginalZwQueryInformationFile(FileHandle,
		IoStatusBlock,
		FileInformation,
		Length,
		FileInformationClass);
	if (!NT_SUCCESS(status))
		 return status;
	if (FileInformationClass == FileStandardInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					fsi = *(FILE_STANDARD_INFORMATION*)FileInformation;
					OutputDebugStringEx("FileStandardInformation获取的文件长度:%d", fsi.EndOfFile.QuadPart);
					if (fsi.EndOfFile.QuadPart> HeadFlaglength)
					{
						((FILE_STANDARD_INFORMATION*)FileInformation)->EndOfFile.QuadPart -= HeadFlaglength;
					}
					mutexObj.unlock();
					return status;
				}
			}
			mutexObj.unlock();
		}
	}
	if (FileInformationClass == FileEndOfFileInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					auto fsiend = *(FILE_END_OF_FILE_INFORMATION*)FileInformation;
					OutputDebugStringEx("FileEndOfFileInformation获取的文件长度:%d", fsiend.EndOfFile.QuadPart);
					((FILE_END_OF_FILE_INFORMATION*)FileInformation)->EndOfFile.QuadPart -= HeadFlaglength;
					mutexObj.unlock();
					return status;
				}
			}
			mutexObj.unlock();
		}
	}
	if (FileInformationClass == FileAllInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					fsiall = *(FILE_ALL_INFORMATION*)FileInformation;
					OutputDebugStringEx("FileAllInformation获取的文件长度:%d", fsiall.StandardInformation.EndOfFile.QuadPart);
					((FILE_ALL_INFORMATION*)FileInformation)->StandardInformation.EndOfFile.QuadPart -= HeadFlaglength;
					mutexObj.unlock();
					return status;
				}
			}
			mutexObj.unlock();
		}
	}
	if (FileInformationClass == FileBothDirectoryInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					auto fsiBOTH = *(FILE_BOTH_DIR_INFORMATION*)FileInformation;
					OutputDebugStringEx("FILE_BOTH_DIR_INFORMATION获取的文件长度:%d", fsiBOTH.EndOfFile.QuadPart);
					((FILE_BOTH_DIR_INFORMATION*)FileInformation)->EndOfFile.QuadPart -= HeadFlaglength;
					mutexObj.unlock();
					return status;
				}
			}
			mutexObj.unlock();
		}
	}
	if (FileInformationClass == FileIdGlobalTxDirectoryInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					auto fsiGlobalTxDirectory = *(FILE_ID_GLOBAL_TX_DIR_INFORMATION*)FileInformation;
					OutputDebugStringEx("FileIdGlobalTxDirectoryInformation获取的文件长度:%d", fsiGlobalTxDirectory.EndOfFile.QuadPart);
					((FILE_ID_GLOBAL_TX_DIR_INFORMATION*)FileInformation)->EndOfFile.QuadPart -= HeadFlaglength;
					mutexObj.unlock();
					return status;
				}
			}
			mutexObj.unlock();
		}
	}
	if (FileInformationClass == FileIdFullDirectoryInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					auto fsiGlobalTxDirectory = *(FILE_ID_FULL_DIR_INFORMATION*)FileInformation;
					OutputDebugStringEx("FileIdFullDirectoryInformation获取的文件长度:%d", fsiGlobalTxDirectory.EndOfFile.QuadPart);
					((FILE_ID_FULL_DIR_INFORMATION*)FileInformation)->EndOfFile.QuadPart -= HeadFlaglength;
					mutexObj.unlock();
					return status;
				}
			}
			mutexObj.unlock();
		}
	}
	if (FileInformationClass == FileNetworkOpenInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					auto fsiGlobalTxDirectory = *(FILE_NETWORK_OPEN_INFORMATION*)FileInformation;
					OutputDebugStringEx("FileNetworkOpenInformation获取的文件长度:%d", fsiGlobalTxDirectory.EndOfFile.QuadPart);
					((FILE_NETWORK_OPEN_INFORMATION*)FileInformation)->EndOfFile.QuadPart -= HeadFlaglength;
					mutexObj.unlock();
					return status;
				}
			}
			mutexObj.unlock();
		}
	}
	if (FileInformationClass == FileIdBothDirectoryInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					auto fsiGlobalTxDirectory = *(FILE_ID_BOTH_DIR_INFORMATION*)FileInformation;
					OutputDebugStringEx("FileIdBothDirectoryInformation获取的文件长度:%d", fsiGlobalTxDirectory.EndOfFile.QuadPart);
					((FILE_ID_BOTH_DIR_INFORMATION*)FileInformation)->EndOfFile.QuadPart -= HeadFlaglength;
					mutexObj.unlock();
					return status;
				}
			}
			mutexObj.unlock();
		}
	}
	if (FileInformationClass == FilePositionInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					if (((PFILE_POSITION_INFORMATION)FileInformation)->CurrentByteOffset.LowPart != -1)
					{
						auto OldQuadPart = ((PFILE_POSITION_INFORMATION)FileInformation)->CurrentByteOffset.QuadPart;
						if (((PFILE_POSITION_INFORMATION)FileInformation)->CurrentByteOffset.QuadPart >= HeadFlaglength)
						{
							((PFILE_POSITION_INFORMATION)FileInformation)->CurrentByteOffset.QuadPart -= HeadFlaglength;
						}
						mutexObj.unlock();
						return status;
					}
				}
			}
			mutexObj.unlock();
		}
	}
	if (FileInformationClass == FileDirectoryInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					auto fsiGlobalTxDirectory = *(FILE_DIRECTORY_INFORMATION*)FileInformation;
					OutputDebugStringEx("FileDirectoryInformation获取的文件长度:%d", fsiGlobalTxDirectory.EndOfFile.QuadPart);
					((FILE_DIRECTORY_INFORMATION*)FileInformation)->EndOfFile.QuadPart -= HeadFlaglength;
					mutexObj.unlock();
					return status;
				}
			}
			mutexObj.unlock();
		}
	}
	if (FileInformationClass == FileFullDirectoryInformation)
	{
		bRet = !m_handleList.empty();
		if (bRet)
		{
			mutexObj.lock();
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == FileHandle)
				{
					auto fsiGlobalTxDirectory = *(FILE_FULL_DIR_INFORMATION*)FileInformation;
					OutputDebugStringEx("FileDirectoryInformation获取的文件长度:%d", fsiGlobalTxDirectory.EndOfFile.QuadPart);
					((FILE_FULL_DIR_INFORMATION*)FileInformation)->EndOfFile.QuadPart -= HeadFlaglength;
					mutexObj.unlock();
					return status;
				}
			}
			mutexObj.unlock();
		}
	}

	return status;
}
