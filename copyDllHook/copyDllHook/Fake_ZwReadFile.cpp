#include "stdafx.h"
#include "copyDllHook.h"
#include "ntstatus.h"
#include <string>
#include "../../OutGoingFileTool/OutGoingFileTool/FIlestruct.h"
#include <mutex>   


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
	std::mutex mutexObj;
	FileHandleRelationNode* pRobj = NULL;
	FILE_POSITION_INFORMATION fpi;
	IO_STATUS_BLOCK iostatus;
	LARGE_INTEGER lOldOffset = { 0 };
	//LARGE_INTEGER lTimeOut = {0};
	LARGE_INTEGER lCurrentOffset = { 0 };
	//char cArrDGKey[HeaderKeyLength] = {0}; //解密key
	bool bRet = FALSE;
	//CRc4 rc4Obj;
	BOOL bOverRideRet = FALSE;
	OVERLAPPED pOverlapped;
	DWORD dwReaded = 0;
	auto HeaderLength = sizeof(RjFileSrtuct) + 1;
	//bRet = !m_handleList.Empty() && m_handleList.Find(FileHandle);
	//OutputDebugStringEx("长度为:%s", HeaderLength);
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
			}
			else
			{
				//pRobj->m_FileInfo.bReadDecrypt = FALSE;
				//bRet = FALSE;
			}
		}
		mutexObj.unlock();
		//m_handleList.Find(*pRobj, FileHandle);
	}

	if (bRet && !pRobj->m_FileInfo.bReadDecrypt)
	{
		//
		// 不需要解密，提示
		//

		delete pRobj;
		//OutputDebugStringEx("不需要解密\r\n");
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
				//OutputDebugStringEx("异步\r\n");
			}
			else if (ByteOffset == NULL) // 同步
			{
				//OutputDebugStringEx("同步\r\n");
				ntStatus = m_pfnOriginalZwQueryInformationFile(FileHandle,
					&iostatus,
					&fpi,    // current pos
					sizeof(FILE_POSITION_INFORMATION),
					FilePositionInformation);

				/*if ((lOldOffset.QuadPart > pRobj->m_FileInfo.liFileSize.QuadPart))
				{
					delete pRobj;

					return m_pfnOriginalZwReadFile(FileHandle,
						Event,
						ApcRoutine,
						ApcContext,
						IoStatusBlock,
						Buffer,
						Length,
						ByteOffset,
						Key);
				}*/
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
				//OutputDebugStringEx("fpi.CurrentByteOffset.QuadPart：%08x\r\n", fpi.CurrentByteOffset.QuadPart);

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

			if (NT_SUCCESS(ntStatus) && NT_SUCCESS(IoStatusBlock->u.Status))
			{
				//OutputDebugStringEx("内容为：%s lCurrentOffset.QuadPart为：%d", Buffer, lCurrentOffset.QuadPart);
				//解密内存
				/*rc4Obj.DecryptMemory(Buffer,
				Buffer,
				lCurrentOffset.LowPart - HeaderLength,
				IoStatusBlock->Information,
				robj.m_FileInfo.Sbox);*/
				/*	if (pRobj->m_FileInfo.rc4Key == NULL)
					{
						pRobj->m_FileInfo.rc4Key = new unsigned char[FileBuffer];
						memset(pRobj->m_FileInfo.rc4Key, 0, FileBuffer);
					}

					rc4Obj.DecryptMemoryRc4K(Buffer,
						Buffer,
						lCurrentOffset.QuadPart - HeaderLength,
						IoStatusBlock->Information,
						pRobj->m_FileInfo.rc4Key);*/
				//OutputDebugStringEx("lCurrentOffset：%08x", lCurrentOffset.QuadPart);
				
			    //MessageBox(NULL, "1111", "dsadsa", MH_OK);

			
					for (int i = 0; i < IoStatusBlock->Information ; i++)
					{
						reinterpret_cast<char*>(Buffer)[i] ^= 'a';
					}


			
				//OutputDebugStringEx("内容为：%s lCurrentOffset.QuadPart为：%d", Buffer, lCurrentOffset.QuadPart);

				// 异步情况
				if (ByteOffset != NULL)  // 恢复位置
				{
					ByteOffset->QuadPart -= HeaderLength;
				}
			}
			delete pRobj;
			return ntStatus;
		} //

	}
	//EXIT:
	//MessageBox(NULL, "1111", "dsadsa", MH_OK);

	ntStatus = m_pfnOriginalZwReadFile(FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		Buffer,
		Length,
		ByteOffset,
		Key);

	if (pRobj!=NULL)
	{
		delete pRobj;
	}
	return ntStatus;
}