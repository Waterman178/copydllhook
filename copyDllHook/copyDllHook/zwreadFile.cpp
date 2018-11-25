#include "stdafx.h"
#include"ntdll.h"


(__stdcall *PIO_APC_ROUTINE) (
	void* ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	long Reserved
	);

typedef struct _FileInfo {
	BOOLEAN bReadDecrypt;
	BOOLEAN bEncryptFile;
	PLARGE_INTEGER liFileSize;
}FileInfo, *pFileInfo;




typedef struct _FileHandleRelationNode {
	FileInfo m_FileInfo;
}FileHandleRelationNode, *pFileHandleRelationNode;


NTSTATUS
WINAPI HookZwReadFile(
	IN HANDLE  FileHandle,
	IN HANDLE  Event  OPTIONAL,
	IN PIO_APC_ROUTINE  ApcRoutine  OPTIONAL,
	IN PVOID  ApcContext  OPTIONAL,
	OUT PIO_STATUS_BLOCK  IoStatusBlock,
	OUT PVOID  Buffer,
	IN ULONG  Length,
	IN PLARGE_INTEGER  ByteOffset  OPTIONAL,
	IN PULONG  Key  OPTIONAL
)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	FileHandleRelationNode* pRobj = NULL;
	FILE_POSITION_INFORMATION fpi;
	FILE_STANDARD_INFORMATION fsi;
	IO_STATUS_BLOCK iostatus;
	LARGE_INTEGER lOldOffset = { 0 };
	//LARGE_INTEGER lTimeOut = {0};
	LARGE_INTEGER lCurrentOffset = { 0 };
	//char cArrDGKey[HeaderKeyLength] = {0}; //解密key
	bool bRet = FALSE;
	CRc4 rc4Obj;
	BOOL bOverRideRet = FALSE;
	OVERLAPPED pOverlapped;
	DWORD dwReaded = 0;

	bRet = !m_handleList.isEmpty() && m_handleList.Find(FileHandle);

	if (bRet)
	{
		pRobj = new FileHandleRelationNode;
		m_handleList.Find(*pRobj, FileHandle);
	}

	if (bRet && !pRobj->m_FileInfo.bReadDecrypt)
	{
		//
		// 不需要解密，提示
		//
		FREE_BUFFER(pRobj);

		return m_pfnOriginalZwReadFile(FileHandle,
			Event,
			ApcRoutine,
			ApcContext,
			IoStatusBlock,
			Buffer,
			Length,
			ByteOffset,
			Key);
	}

	if (bRet)
	{
		if (pRobj->m_FileInfo.bEncryptFile)
		{
			// 异步
			if (ByteOffset != NULL)
			{
				ByteOffset->QuadPart += HeaderLength;
				lCurrentOffset.QuadPart = ByteOffset->QuadPart;
			}
			else if (ByteOffset == NULL) // 同步
			{
				ntStatus = m_pfnOriginalZwQueryInformationFile(FileHandle,
					&iostatus,
					&fpi,    // current pos
					sizeof(FILE_POSITION_INFORMATION),
					FilePositionInformation);

				if (lOldOffset.QuadPart > pRobj->m_FileInfo.liFileSize.QuadPart)
				{
					FREE_BUFFER(pRobj);

					return m_pfnOriginalZwReadFile(FileHandle,
						Event,
						ApcRoutine,
						ApcContext,
						IoStatusBlock,
						Buffer,
						Length,
						ByteOffset,
						Key);
				}
				if (fpi.CurrentByteOffset.QuadPart < HeaderLength)
				{
					fpi.CurrentByteOffset.QuadPart += HeaderLength;

					ntStatus = m_pfnOriginalZwSetInformationFile(FileHandle,
						&iostatus,
						&fpi,    // +1024
						sizeof(FILE_POSITION_INFORMATION),
						FilePositionInformation);
				}

				lCurrentOffset.QuadPart = fpi.CurrentByteOffset.QuadPart;

			}

			// 当前位置 < 1024  pos + 1024
			ntStatus = m_pfnOriginalZwReadFile(FileHandle,
				Event,
				ApcRoutine,
				ApcContext,
				IoStatusBlock,
				Buffer,
				Length,
				ByteOffset,
				Key);


			if (ByteOffset != NULL)
			{
				if (ApcContext != NULL)
				{
					pOverlapped = *(OVERLAPPED*)ApcContext;
				}
				else
				{
					pOverlapped.Internal = STATUS_PENDING;
					pOverlapped.hEvent = Event;
					pOverlapped.Offset = ByteOffset->u.LowPart;
					pOverlapped.OffsetHigh = ByteOffset->u.HighPart;
				}

				//powerpoint
				//if (m_pStrategy->IsPowerPoint())
				//{
				//	if (ntStatus == STATUS_PENDING)
				//	{
				//		//bOverRideRet = GetOverlappedResult(FileHandle, &pOverlapped, &dwReaded, FALSE);
				//		CancelIo(FileHandle);
				//	}
				//}
				//else
				//{
				//	bOverRideRet = GetOverlappedResult(FileHandle, &pOverlapped, &dwReaded, TRUE);
				//}

				if (Event && ntStatus == STATUS_PENDING)
				{
					bOverRideRet = GetOverlappedResult(FileHandle, &pOverlapped, &dwReaded, TRUE);
					SetEvent(Event);
				}
				else
				{
					bOverRideRet = GetOverlappedResult(FileHandle, &pOverlapped, &dwReaded, TRUE);
				}

				//OutputLogMsg(LOGLEVEL_INFO, L"[%s] %d %s 0x%08x [IO]0x%08x %d", __FUNCTIONW__, FileHandle, robj.m_FileInfo.wcSrcFileName, ntStatus, IoStatusBlock->Status, bOverRideRet);
			}

			if (NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatusBlock->Status))
			{
				//解密内存
				/*rc4Obj.DecryptMemory(Buffer,
				Buffer,
				lCurrentOffset.LowPart - HeaderLength,
				IoStatusBlock->Information,
				robj.m_FileInfo.Sbox);*/
				if (pRobj->m_FileInfo.rc4Key == NULL)
				{
					pRobj->m_FileInfo.rc4Key = new unsigned char[FileBuffer];
					memset(pRobj->m_FileInfo.rc4Key, 0, FileBuffer);
				}

				rc4Obj.DecryptMemoryRc4K(Buffer,
					Buffer,
					lCurrentOffset.QuadPart - HeaderLength,
					IoStatusBlock->Information,
					pRobj->m_FileInfo.rc4Key);

				// 异步情况
				if (ByteOffset != NULL)  // 恢复位置
				{
					ByteOffset->QuadPart -= HeaderLength;
				}
			}
			FREE_BUFFER(pRobj);

			return ntStatus;
		} //

	}
	//EXIT:
	ntStatus = m_pfnOriginalZwReadFile(FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		Buffer,
		Length,
		ByteOffset,
		Key);

	FREE_BUFFER(pRobj);

	return ntStatus;
}