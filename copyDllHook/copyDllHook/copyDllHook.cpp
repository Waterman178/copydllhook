// copyDllHook.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "copyDllHook.h"
#include "DebugPrintf.h"
#include "GetParentProID.h"
#include "GlobalHook.h"

#define FILE_SIGN "DSKFJLSKDF"
#define FILE_SIGN_LEN 10

//ȫ�ֱ���
std::list<HANDLE> MAPHAD_list;
std::list<HANDLE>::iterator map_ite;
HANDLE  dochFile = (HANDLE)-1;//word�ļ����ļ��ľ��

HANDLE hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, _T("processMem_FUCK"));
PVOID pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

//void OutputDebugStringExW(const wchar_t *strOutputString, ...)
//
//{
//
//	va_list vlArgs = NULL;
//
//	va_start(vlArgs, strOutputString);
//
//	size_t nLen = _vscwprintf(strOutputString, vlArgs) + 1;
//
//	wchar_t *strBuffer = new wchar_t[nLen];
//
//	_vsnwprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
//
//	va_end(vlArgs);
//
//	OutputDebugStringW(strBuffer);
//
//	delete[] strBuffer;
//
//}



void OutputDebugStringEx(const char *strOutputString, ...)

{

	va_list vlArgs = NULL;

	va_start(vlArgs, strOutputString);

	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1;

	char *strBuffer = new char[nLen];

	_vsnprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);

	va_end(vlArgs);

	OutputDebugStringA(strBuffer);

	delete[] strBuffer;

}
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>��HOOK�ĺ������¹���<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
/***********************************************
OpenClipboard�򿪼��а�
************************************************/
static BOOL WINAPI NewOpenClipboard(
	_In_opt_ HWND hWndNewOwner
	)
{
	if (GetParentProcessID(GetCurrentProcessId()) == *(int*)pBuffer)
	{
		return FALSE;
	}
	return openClipboard(hWndNewOwner);
	
}

/***********************************************
GetClipboardData()������ȡ���а������
************************************************/
static HANDLE WINAPI NewGetClipboardData(
	_In_ UINT uFormat
	)
{
	if (GetParentProcessID(GetCurrentProcessId()) == *(int*)pBuffer)
	{
		return NULL;
	}
	return getClipboardData(uFormat);
}

/***********************************************
OpenPrinter()���� ��ӡ����
************************************************/
static BOOL WINAPI NewOpenPrinter(__in LPTSTR pPrinterName,
	__out LPHANDLE phPrinter,
	__in LPPRINTER_DEFAULTS pDefault
	)
{
	if (GetParentProcessID(GetCurrentProcessId()) == *(int*)pBuffer)
	{
		return FALSE;
	}
	return openPrinter(pPrinterName, phPrinter, pDefault);
}

/***********************************************
showWindow()���� ����Ϊ����
************************************************/
static BOOL WINAPI NewShowWindow(HWND hWnd, int nCmdShow)
{
	//CopyFileExW
	//OutputDebugStringEx("HOOK  showWindow: hwnd = %d,nCmdShow = %d", hWnd, nCmdShow);
	return showWindow(hWnd, nCmdShow);
}
/***********************************************
SetWindowPos()���� �ı�һ���Ӵ��ڣ�
����ʽ���ڻ򶥲㴰�ڵĳߴ磬λ�ú�Z����
************************************************/
static BOOL WINAPI NewSetWindowPos(_In_     HWND hWnd,//��z���е�λ�ڱ���λ�Ĵ���ǰ�Ĵ��ھ�����ò�������Ϊһ�����ھ��
	_In_opt_ HWND hWndInsertAfter,//���ڱ�ʶ��z-˳��Ĵ� CWnd ����֮ǰ�� CWnd ����
	_In_     int  X,//�Կͻ�����ָ��������λ�õ���߽硣
	_In_     int  Y,//�Կͻ�����ָ��������λ�õĶ��߽硣
	_In_     int  cx,//������ָ�����ڵ��µĿ��ȡ�
	_In_     int  cy,//������ָ�����ڵ��µĸ߶ȡ�
	_In_     UINT uFlags//���ڳߴ�Ͷ�λ�ı�־,15�ֲ���
	)
{
	
	//OutputDebugStringEx("HOOK  setWindow: hwnd = %d,nCmdShow = %d ,uFlag = %d", hWnd, hWndInsertAfter, uFlags);
	return setWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}
/***********************************************
SetWindowTextW()����  SetWindowTextA()���� 
���öԻ��������߶Ի���ؼ��ı�������
************************************************/
static BOOL WINAPI NewSetWindowTextW(HWND hwnd, //Ҫ�ı��ı����ݵĴ��ڻ�ؼ��ľ��
	LPCTSTR lpString)//ָ��һ���ս������ַ�����ָ�룬���ַ�������Ϊ���ڻ�ؼ������ı�
{
	//OutputDebugStringEx("HOOK  setWindowW: hwnd = %d,nCmdShow = %s", hwnd, lpString);
	CString otherSave = _T("����Ϊ");
	if (0 == memcmp(otherSave.GetBuffer(), lpString, lstrlenW(lpString)))
	{
		//OutputDebugStringEx("����:return False>>>����Ϊ ");
		//CloseWindow(hwnd);
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}
	otherSave = _T("����Ϊ PDF �� XPS");
	if (0 == memcmp(otherSave.GetBuffer(), lpString, lstrlenW(lpString)))
	{
		//OutputDebugStringEx("����:return False>>>����Ϊ ");
		//CloseWindow(hwnd);
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}
	otherSave = _T("����(&S)");
	if (0 == memcmp(otherSave.GetBuffer(), lpString, lstrlenW(lpString)))
	{
		//OutputDebugStringEx("HOOK����:return False>>>���� ");
		//CloseWindow(hwnd);
		return setWindowTextW(hwnd, NULL);
	}
	return setWindowTextW(hwnd, lpString);
}

static BOOL WINAPI NewSetWindowTextA(HWND hwnd, //Ҫ�ı��ı����ݵĴ��ڻ�ؼ��ľ��
	LPCTSTR lpString)//ָ��һ���ս������ַ�����ָ�룬���ַ�������Ϊ���ڻ�ؼ������ı�
{
	//OutputDebugStringEx("HOOK  setWindowA: hwnd = %d,nCmdShow = %s", hwnd, lpString);
	return setWindowTextA(hwnd, lpString);
}

/***********************************************
CreateFileW()����			��������������ж���
************************************************/
static HANDLE WINAPI NewCreateFileW(
	_In_     LPCTSTR               lpFileName,
	_In_     DWORD                 dwDesiredAccess,
	_In_     DWORD                 dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_     DWORD                 dwCreationDisposition,
	_In_     DWORD                 dwFlagsAndAttributes,
	_In_opt_ HANDLE                hTemplateFile)
{

	if (memcmp(lpFileName, _T("\\\\"), 4) != 0) {
		HANDLE keyHan = createFileW(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		//OutputDebugStringEx(">>>>>>>>HOOK THE NewCreateFileW %d %s", keyHan, lpFileName);
		if (keyHan != INVALID_HANDLE_VALUE) {
			DWORD readLen;
			LPVOID fileHead = new char[FILE_SIGN_LEN];
			int currentPointer = 0;
			//currentPointer = setFilePointer(keyHan, 0, NULL, FILE_CURRENT);
			setFilePointer(keyHan, -FILE_SIGN_LEN, NULL, FILE_END);
			readfile(keyHan, fileHead, FILE_SIGN_LEN, &readLen, NULL);
			/*setFilePointer(keyHan, currentPointer, NULL, FILE_BEGIN)*/;
			//OutputDebugStringEx("******HOOK: fileHead = %s", fileHead);
			closeHandle(keyHan);
			if (memcmp(fileHead, FILE_SIGN, FILE_SIGN_LEN) == 0)
			{
				OutputDebugStringEx("**************HOOK:sercret file\r\n ");
				dwDesiredAccess &= ~GENERIC_WRITE;
			}
			delete fileHead;
		}
	}
	HANDLE ret = createFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	
	OutputDebugStringEx(">>>>>>>>HOOK THE NewCreateFileW: %s  ,  %d\r\n", lpFileName, ret);
	return ret;
}

/***********************************************
Readfile()����			������ȡ�ļ�����
************************************************/
static int WINAPI NewReadfile(
	_In_        HANDLE       hFile,
	_Out_       LPVOID       lpBuffer,
	_In_        DWORD        nNumberOfBytesToRead,
	_Out_opt_   LPDWORD      lpNumberOfBytesRead,
	_Inout_opt_ LPOVERLAPPED lpOverlapped)
{
	OutputDebugStringEx(">>>>>>>HOOK:   readfile function !!!\r\n");
	int Ret = 0;
	DWORD readLen;
	LPVOID fileHead = new char[FILE_SIGN_LEN];
	int currentPointer = 0;
	currentPointer = setFilePointer(hFile, 0, NULL, FILE_CURRENT);
	setFilePointer(hFile, -FILE_SIGN_LEN, NULL, FILE_END);
	readfile(hFile, fileHead, FILE_SIGN_LEN, &readLen, lpOverlapped);
	setFilePointer(hFile, currentPointer, NULL, FILE_BEGIN);
	OutputDebugStringEx("$$$$$$$$$HOOK : THE FILE currentPointer = %d,   nNumberOfBytesToRead=%d,getfilesize=%d\r\n", currentPointer, nNumberOfBytesToRead, getFileSize(hFile, NULL));
	if (memcmp(fileHead, FILE_SIGN, FILE_SIGN_LEN) == 0)
	{
		dochFile = hFile;
		if (currentPointer + nNumberOfBytesToRead > NewGetFileSize(hFile, NULL))
		{

			OutputDebugStringEx(">>>>>>>HOOK:  currentPointer + nNumberOfBytesToRead > getFileSize(hFile, NULL) - 10!!!\r\n");
			DWORD  temp = NewGetFileSize(hFile, NULL) - currentPointer;
			if (temp > 0)
			{
				nNumberOfBytesToRead = temp;
			}
		}
		
		OutputDebugStringEx("$$$$$********$$$$$ HOOK!!!!!currentPointer = %d,nNumberOfBytesToRead=%d\r\n", currentPointer, nNumberOfBytesToRead);
		Ret = readfile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
		for (int i = 0; i < *lpNumberOfBytesRead; i++) {
			((char *)lpBuffer)[i] ^= 'a';
		}

		//TODO:�������������ϢҪ���д���һ��

		//hexdump((unsigned char *)lpBuffer, nNumberOfBytesToRead > 10 ? 10 : nNumberOfBytesToRead);
	}
	else
	{		
		Ret = readfile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	}
	delete fileHead;
	OutputDebugStringEx(">>>>>>>HOOK:::  Readfile  Ret = %d\r\n", Ret);
	return Ret;
}

/***********************************************
Weadfile()����			����д�ļ�����
************************************************/
static BOOL WINAPI New_WriteFile(
	HANDLE hFile, LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{
	//OutputDebugStringEx(">>>>>>>>HOOK THE WRITEFILE hFile=%d,dochFile=%d", hFile, dochFile);

		// char *aa = new char[nNumberOfBytesToRead];
		//rc4(FILE_SIGN, FILE_SIGN_LEN, (char *)lpBuffer, nNumberOfBytesToWrite, (char *)lpBuffer);
		//lpBuffer = aa;
	
	return writeFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}
/***********************************************
MapViewOfFile()����			�ļ�ӳ�����ӳ�䵽��ǰӦ�ó���ĵ�ַ�ռ书��
************************************************/
static LPVOID WINAPI NewMapViewOfFile(
	__in HANDLE hFileMappingObject,
	__in DWORD dwDesiredAccess,
	__in DWORD dwFileOffsetHigh,
	__in DWORD dwFileOffsetLow,
	__in SIZE_T dwNumberOfBytesToMap
	)
{

	OutputDebugStringEx(">>>>>>>>HOOK THE MapViewOfFile buf %d \r\n", hFileMappingObject);

	LPVOID pMsg = mapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap);		
	LPVOID Ret = pMsg;
	if (MAPHAD_list.empty())
	{
		return pMsg;
	}
	else
	{
		for (map_ite = MAPHAD_list.begin(); map_ite != MAPHAD_list.end(); map_ite++)
		{
			if (hFileMappingObject == *map_ite)
			{
				OutputDebugStringEx("@@@@@@@@HOOK: THIS IS A SECRET FILE!!!!  dwNumberOfBytesToMap = %d \r\n", dwNumberOfBytesToMap);
				char *aa = new char[dwNumberOfBytesToMap];
				int i = 0;
				for (; i < (dwNumberOfBytesToMap - 10); i++) {
					aa[i] = ((char *)Ret)[i] ^ 'a';
				}
				for (;i < dwNumberOfBytesToMap; i++)
				{
					aa[i] = '\0';
				}
				Ret = aa;
				return Ret;
			}
		}
	}
	return pMsg;
}
static LPVOID WINAPI NewMapViewOfFileEx(
	__in HANDLE hFileMappingObject,
	__in DWORD dwDesiredAccess,
	__in DWORD dwFileOffsetHigh,
	__in DWORD dwFileOffsetLow,
	__in SIZE_T dwNumberOfBytesToMap,
	__in LPVOID lpBaseAddress
	)
{
	//char* buf = (char *)mapViewOfFileEx(hFileMappingObject, dwDesiredAccess, 0, 30, 30, lpBaseAddress);

	//OutputDebugStringEx(">>>>>>>>HOOK THE MapViewOfFileEx buf = %s",buf);
//	OutputDebugStringEx("hFileMappingObject = %d,dwDesiredAccess=%d,dwFileOffsetHigh=%d", hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh);
//	OutputDebugStringEx("dwFileOffsetLow = %d,dwNumberOfBytesToMap=%d,lpBaseAddress=%d", dwFileOffsetLow, dwNumberOfBytesToMap, lpBaseAddress);
	return mapViewOfFileEx(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap, lpBaseAddress);
}
/***********************************************
CreateFileMapping()����			����һ���µ��ļ�ӳ���ں˶�����
************************************************/
static HANDLE WINAPI NewCreateFileMapping(
	_In_ HANDLE hFile,
	_In_opt_ LPSECURITY_ATTRIBUTES lpAttributes,
	_In_ DWORD flProtect,
	_In_ DWORD dwMaximumSizeHigh,
	_In_ DWORD dwMaximumSizeLow,
	_In_opt_ LPCTSTR lpName)
{
	OutputDebugStringEx(">>>>>>>>HOOK THE CreateFileMapping %d\r\n", hFile);
	HANDLE fileMap = createFileMapping(hFile, lpAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
	DWORD readLen;
	LPVOID fileHead = new char[FILE_SIGN_LEN];
	int currentPointer = 0;
	currentPointer = setFilePointer(hFile, 0, NULL, FILE_CURRENT);
	setFilePointer(hFile, -FILE_SIGN_LEN, NULL, FILE_END);
	readfile(hFile, fileHead, FILE_SIGN_LEN, &readLen, NULL);
	setFilePointer(hFile, currentPointer, NULL, FILE_BEGIN);
	//OutputDebugStringEx("******HOOK: fileHead = %s", fileHead);
	if (memcmp(fileHead, FILE_SIGN, FILE_SIGN_LEN) == 0)
	{
		OutputDebugStringEx("**************HOOK:sercret file \r\n");
		MAPHAD_list.push_back(fileMap);
	}
	delete fileHead;
	return fileMap;
}
/***********************************************
OpenFileMappingW()����			��һ���µ��ļ�ӳ���ں˶�����
************************************************/
static HANDLE WINAPI NewOpenFileMappingW(
	_In_ DWORD   dwDesiredAccess,
	_In_ BOOL    bInheritHandle,
	_In_ LPCTSTR lpName)
{
	OutputDebugStringEx(">>>>>>>>HOOK THE OpenFileMappingW\r\n");
	OutputDebugStringEx(">HOOK< dwDesiredAccess = %d,bInheritHandle=%d,lpName=%s\r\n", dwDesiredAccess, bInheritHandle, lpName);
	return openFileMappingW(dwDesiredAccess, bInheritHandle, lpName);
}
/***********************************************
ZwClose()����			���ݾ���رչ���
************************************************/
static NTSTATUS WINAPI New_ZwClose(_In_ HANDLE Handle)
{	
	NTSTATUS Ret = zwClose(Handle);
	return Ret;
}
static NTSTATUS NTAPI NewNtClose(IN HANDLE Handle)
{
	NTSTATUS Ret = ntClose(Handle);
	
	return Ret;
}
/***********************************************
SetFilePointer()����			���ݾ���رչ���
************************************************/
static DWORD WINAPI NewSetFilePointer(
	_In_        HANDLE hFile,
	_In_        LONG   lDistanceToMove,
	_Inout_opt_ PLONG  lpDistanceToMoveHigh,
	_In_        DWORD  dwMoveMethod
	)
{
	OutputDebugStringEx("$$$$$>>>>>   3   HOOK:  THE SetFilePointer function !!!!,lDistanceToMove = %d\r\n", lpDistanceToMoveHigh);
	if (dwMoveMethod == FILE_END && hFile == dochFile)
	{	
		lDistanceToMove -= FILE_SIGN_LEN;
	}
	
	return setFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
}
/***********************************************
GetFileInformationByHandle()����			���ݾ���رչ���
************************************************/
static BOOL WINAPI NewGetFileInformationByHandle(
	_In_  HANDLE                       hFile,
	_Out_ LPBY_HANDLE_FILE_INFORMATION lpFileInformation)
{
	OutputDebugStringEx("@@@@@@@@@@HOOK:  THE GETFILEINFORMATIONBYHANDLE FUNCTION !!     %d\r\n",lpFileInformation->dwFileAttributes);
	
	BOOL Ret = getFileInformationByHandle(hFile, lpFileInformation);
	//lpFileInformation->dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
	return Ret;
}
/***********************************************
GetFileAttributesW()����			��ȡ�ļ�����
************************************************/
static DWORD WINAPI NewGetFileAttributesW(
	_In_ LPCTSTR lpFileName)
{
	DWORD Ret = getFileAttributesW(lpFileName);

	//OutputDebugStringEx("@@@@@@@@@HOOK: GetFileAttributesW FUNCTION lpFileName=%s Attrib=%d", lpFileName, Ret);
	//Ret |= FILE_ATTRIBUTE_READONLY;
	return Ret;
}
/***********************************************
ReadFileEx()����			��ȡ�ļ�����
************************************************/
static BOOL WINAPI NewReadFileEx(
	_In_      HANDLE                          hFile,
	_Out_opt_ LPVOID                          lpBuffer,
	_In_      DWORD                           nNumberOfBytesToRead,
	_Inout_   LPOVERLAPPED                    lpOverlapped,
	_In_      LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	)
{
	OutputDebugStringEx("HOOK:  THE NewReadFileEx FUNCTION !!\r\n");
	BOOL result = readFileEx(hFile, lpBuffer, nNumberOfBytesToRead, lpOverlapped, lpCompletionRoutine);
	return result;
}
/***********************************************
ReadFileScatter()����			��ȡ�ļ�����
************************************************/
static BOOL WINAPI NewReadFileScatter(
	_In_       HANDLE               hFile,
	_In_       FILE_SEGMENT_ELEMENT aSegmentArray[],
	_In_       DWORD                nNumberOfBytesToRead,
	_Reserved_ LPDWORD              lpReserved,
	_Inout_    LPOVERLAPPED         lpOverlapped
	)
{
	OutputDebugStringEx("HOOK:  THE NewReadFileScatter FUNCTION !!\r\n");
	BOOL result = readFileScatter(hFile, aSegmentArray, nNumberOfBytesToRead, lpReserved, lpOverlapped);
	return result;
}
/***********************************************
GetFileAttributesExW()����			��ȡ�ļ�����
************************************************/
static DWORD WINAPI NewGetFileAttributesExW(
	__in LPCTSTR lpFileName
	)
{
	OutputDebugStringEx("HOOK:  THE NewGetFileAttributesExW FUNCTION !!\r\n");
	DWORD result = getFileAttributesExW(lpFileName);
	return result;
}
/***********************************************
GetFileSizeEx()����			����
************************************************/
static BOOL WINAPI NewGetFileSizeEx(
	_In_  HANDLE         hFile,
	_Out_ PLARGE_INTEGER lpFileSize)
{
	
	BOOL result = getFileSizeEx(hFile, lpFileSize);
	DWORD readLen;
	LPVOID fileHead = new char[FILE_SIGN_LEN];
	int currentPointer = 0;
	currentPointer = setFilePointer(hFile, 0, NULL, FILE_CURRENT);
	setFilePointer(hFile, -FILE_SIGN_LEN, NULL, FILE_END);
	readfile(hFile, fileHead, FILE_SIGN_LEN, &readLen, NULL);
	setFilePointer(hFile, currentPointer, NULL, FILE_BEGIN);
	OutputDebugStringEx("HOOK:  THE NewGetFileSizeEx FUNCTION !!    readHead = %s\r\n", fileHead);
	if (memcmp(fileHead, FILE_SIGN, FILE_SIGN_LEN) == 0)
	{		
		OutputDebugStringEx("********  getFileSizeEx ******HOOK:sercret file \r\n");	
		lpFileSize->QuadPart -= FILE_SIGN_LEN;
	}
	OutputDebugStringEx("HOOK:  THE NewGetFileSizeEx FUNCTION !!  %ld\r\n ", lpFileSize->QuadPart);
	delete fileHead;
	return result;
}
/***********************************************
SetFilePointerEx()����			����
************************************************/
static BOOL WINAPI NewSetFilePointerEx(
	_In_      HANDLE         hFile,
	_In_      LARGE_INTEGER  liDistanceToMove,
	_Out_opt_ PLARGE_INTEGER lpNewFilePointer,
	_In_      DWORD          dwMoveMethod)
{
	OutputDebugStringEx("HOOK: THE SetFilePointerEx FUNCTION!!!!hFile=%d, liDistanceToMove=%d\r\n", hFile, liDistanceToMove);
	if (dwMoveMethod == FILE_END && hFile == dochFile)
	{
		liDistanceToMove.QuadPart -= FILE_SIGN_LEN;
	}
	return setFilePointerEx(hFile, liDistanceToMove, lpNewFilePointer, dwMoveMethod);
}
/***********************************************
GetFileSize()����			A��ȡ�ļ����ȹ���
************************************************/
static DWORD WINAPI NewGetFileSize(
	HANDLE hFile,
	LPDWORD lpFileSizeHigh)
{
	OutputDebugStringEx(">>>>>>>>>>>>>>>>>>>HOOK : THE Getfilesize Function!!!!\r\n");
	if (hFile == dochFile)
	{
		OutputDebugStringEx("********  NewGetFileSize ******HOOK:sercret file\r\n ");
		return getFileSize(hFile, NULL) - FILE_SIGN_LEN;
	}
	return getFileSize(hFile, lpFileSizeHigh);
}
/***********************************************
GetSaveFileNameW()����			A��ȡ�ر��ļ��Ự��
************************************************/
static BOOL WINAPI New_GetSaveFileNameW(
	_Inout_ LPOPENFILENAME lpofn)
{
	//OutputDebugStringEx("@@@@@@@@@@HOOK:  GetSaveFileNameW FUNCTION!!!  ");
	BOOL Ret = getSaveFileNameW(lpofn);
	return Ret;
}
static BOOL WINAPI NewCloseHandle(HANDLE hObject)
{
	//OutputDebugStringEx("HOOK:  THE CLOSEHANDLE FUNCTION !!!!  handle = %d", hObject);
	return closeHandle(hObject);
}
/***********************************************
UnmapViewOfFile()����			ֹͣ�ڴ�ӳ�书��
************************************************/
static BOOL WINAPI NewUnmapViewOfFile(_In_ LPCVOID lpBaseAddress)
{
	return unmapViewOfFile(lpBaseAddress);
}
/***********************************************
BitBlt()����			������ͼ����
************************************************/
static BOOL WINAPI New_BitBlt(_In_ HDC   hdcDest,
	_In_ int   nXDest, _In_ int   nYDest, _In_ int   nWidth,
	_In_ int   nHeight, _In_ HDC   hdcSrc, _In_ int   nXSrc,
	_In_ int   nYSrc, _In_ DWORD dwRop
	)
{
	int result;
	//OutputDebugStringEx("@@@@@@@@@@HOOK:  BitBlt FUNCTION!!!  ");
	if ((dwRop & 0xCC0020) == 0xCC0020 && GetObjectType(hdcSrc) == 3)
	{		
		HWND gameh = FindWindow(NULL, TEXT("OutGoingFileTool"));
		if (gameh == 0)
		{
			result = bitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
		}
		else
		{
			result = bitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, BLACKNESS);
		}	
	}
	else
	{
		result = bitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
	}
	return result;
}
/***********************************************
StretchBlt()����			������ͼ����
************************************************/
static BOOL WINAPI New_StretchBlt(HDC hdcDest, int nXOriginDest,
	int nYOriginDest, int nWidthDest,
	int nHeightDest, HDC hdcSrc,
	int nXOriginSrc, int nYOriginSrc, int nWidthSrc,
	int nHeightSrc, DWORD dwRop)
{
	//OutputDebugStringEx("HOOK����2New_StretchBlt�� hdcDest = %d,hdcSrc = %d,dwRop = %d", hdcDest, hdcSrc, dwRop);
	return stretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop);
}

/***********************************************
CreateProcessW()����			�������̹���
************************************************/
static BOOL WINAPI NewCreateProcessW(
	_In_opt_    LPCTSTR               lpApplicationName,
	_Inout_opt_ LPTSTR                lpCommandLine,
	_In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_        BOOL                  bInheritHandles,
	_In_        DWORD                 dwCreationFlags,
	_In_opt_    LPVOID                lpEnvironment,
	_In_opt_    LPCTSTR               lpCurrentDirectory,
	_In_        LPSTARTUPINFO         lpStartupInfo,
	_Out_       LPPROCESS_INFORMATION lpProcessInformation
	)
{
	OutputDebugStringEx("???????????HOOK: THE NewCreateProcessW FUNCTION !!!!\r\n");
	BOOL result;
	result = createProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

	//InjectDll(lpProcessInformation->dwProcessId, _T("E:\\�ⷢ����\\HookProject_test\\fileHook\\OutGoingFileTool\\Debug\\copyDllHook.dll"));
		
	return result;
}
/***********************************************
CreateProcessInternal()����			�������̹���
************************************************/
static HANDLE WINAPI NewCreateProcessInternal(
	HANDLE hToken, 
	LPCTSTR lpApplicationName, 
	LPTSTR lpCommandLine, 
	LPSECURITY_ATTRIBUTES lpProcessAttributes, 
	LPSECURITY_ATTRIBUTES lpThreadAttributes, 
	BOOL bInheritHandles, 
	DWORD dwCreationFlags, 
	LPVOID lpEnvironment, 
	LPCTSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation, 
	PHANDLE hNewToken)
{
	OutputDebugStringEx("???????????HOOK: THE NewCreateProcessInternal FUNCTION !!!! name = %s\r\n", lpApplicationName);
	HANDLE result;
	result = createProcessInternalW(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
							dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);
	InjectDll(lpProcessInformation->dwProcessId, _T("E:\\�ⷢ����\\HookProject_test\\fileHook\\OutGoingFileTool\\Debug\\copyDllHook.dll\r\n"));

	return result;
}


/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>HOOK���ܼ�����<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/***********************************************

************************************************/
PVOID FindProcAddress(_In_ LPCTSTR lpFileName, LPCSTR lpProcName)
{
	PVOID insthook;
	HINSTANCE hinst = NULL;
	hinst = LoadLibrary(lpFileName);//����dll�ļ�
	if (hinst == NULL)
	{
		return NULL;
	}
	insthook = GetProcAddress(hinst, lpProcName);//��ȡ������ַ
	if (insthook == NULL)
	{
		return NULL;
	}
	return insthook;
}
/***********************************************
Function: StartOneHook

Description: ��ʼHOOKһ������

Input:	LPCTSTR dllName �������ڵ�dll��
		LPCSTR funcName ��������
		PVOID newFunc	�º����ĺ���ָ��
Returns: �ɹ����ɺ�����ַ ʧ�ܣ�NULL
************************************************/
PVOID StartOneHook(LPCTSTR dllName, LPCSTR funcName,PVOID newFunc)
{
	//��ʼ����
	DetourTransactionBegin();
	//�����߳���Ϣ  
	DetourUpdateThread(GetCurrentThread());
	PVOID oldFunc = FindProcAddress(dllName,funcName);
	if (oldFunc == NULL)
	{
		return NULL;
	}
	//�����صĺ������ӵ�ԭ�����ĵ�ַ��
	DetourAttach(&oldFunc, newFunc);
	//��������
	DetourTransactionCommit();
	return oldFunc;
}
/***********************************************
Function: EndOneHook

Description: ֹͣHOOKһ������

Input:	LPCTSTR dllName �������ڵ�dll��
		LPCSTR funcName ��������
		PVOID newFunc	�º����ĺ���ָ��

************************************************/
void EndOneHook(LPCTSTR dllName, PVOID oldFunc, PVOID newFunc)
{
	////OutputDebugStringEx("end the funcName=%s ", funcName);
	//��ʼ����
	DetourTransactionBegin();
	//�����߳���Ϣ 
	DetourUpdateThread(GetCurrentThread());
	//�����صĺ�����ԭ�����ĵ�ַ�Ͻ��
	//PVOID oldFunc = FindProcAddress(dllName,funcName);
	//if (oldFunc == NULL)
	//{
	//	return;
	//}
	DetourDetach(&oldFunc, newFunc);
	//��������
	DetourTransactionCommit();
}
/***********************************************
Function: rc4

Description: rc4�����㷨

Input:	char *pSecret KEY
		int SecretLen ��Կ����
		char *pMessage ������Ϣ
		int MessageLen ������Ϣ����
		char *pOut ������ܺ����Ϣ
************************************************/
int rc4(char *pSecret, int SecretLen, char *pMessage, int MessageLen, char *pOut)
{
	int ia;
	unsigned char i = 0, j = 0, t;
	unsigned char s[256];
	unsigned char k[256];

	for (ia = 0; ia <= 255; ia++, i++)
		s[ia] = i;

	for (ia = 0; ia <= 255; ia++)
		k[ia] = ((unsigned char *)pSecret)[ia%SecretLen];
	for (ia = i = j = 0; ia <= 255; ia++, i++)
	{
		j = (j + s[i] + k[i]) % 256;
		t = s[i]; s[i] = s[j]; s[j] = t;
	}
	for (ia = i = j = 0; ia <= MessageLen - 1; ia++)
	{
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		t = s[i]; s[i] = s[j]; s[j] = t;
		t = (s[i] + s[j]) % 256;
		((unsigned char *)pOut)[ia] = s[t] ^ ((unsigned char *)pMessage)[ia];
	}
	return 0;
}
void hexdump(const unsigned char *buf, const int num)
{
	char *temp = new char[num * 3 + 20];
	char *temp1 = new char[num * 3 * 2 + 20];
	memset(temp1, 0, num * 3 * 2 + 20);
	//OutputDebugStringEx("HOOK START:\n");
	int i;
	for (i = 0; i < num; i++)
	{
		//OutputDebugStringEx("HOOK %d:%02x ", i, buf[i]);
		sprintf_s(temp + i * 3, 4, "%02X \r\n", buf[i]);
	}
	for (i = 0; i < num * 3; i++) {
		temp1[i * 2] = temp[i];
	}
	OutputDebugStringEx("HOOK DUMP!!! %s\r\n", temp1);
	delete temp;
	delete temp1;

	return;
}
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>DLL������������<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
/***********************************************
Function: StartHook

Description: ��ʼ���е�HOOK

************************************************/
void __stdcall StartHook()
{
		openClipboard = OPENCLIPBOARD StartOneHook(USER32, "OpenClipboard", NewOpenClipboard);
		openPrinter = OPENPRINTER StartOneHook(WINSPOOL, "OpenPrinterW", NewOpenPrinter);
		showWindow = SHOWWINDOW StartOneHook(USER32, "ShowWindow", NewShowWindow);
		setWindowPos = SETWINDOWPOS StartOneHook(USER32, "SetWindowPos", NewSetWindowPos);
		setWindowTextW = SETWINDOWTEXTW StartOneHook(USER32, "SetWindowTextW", NewSetWindowTextW);
		setWindowTextA = SETWINDOWTEXTA StartOneHook(USER32, "SetWindowTextA", NewSetWindowTextA);
		createFileW = CREATEFILEW StartOneHook(KERNEL32, "CreateFileW", NewCreateFileW);
		readfile = READFILE StartOneHook(KERNEL32, "ReadFile", NewReadfile);
		writeFile = WRITEFILE StartOneHook(KERNEL32, "WriteFile", New_WriteFile);
		mapViewOfFile = MAPVIEWOFFILE StartOneHook(KERNEL32, "MapViewOfFile", NewMapViewOfFile);
		mapViewOfFileEx = MAPVIEWOFFILEEX StartOneHook(KERNEL32, "MapViewOfFileEx", NewMapViewOfFileEx);
		createFileMapping = CREATEFILEMAPPING StartOneHook(KERNEL32, "CreateFileMappingW", NewCreateFileMapping);
		openFileMappingW = OPENFILEMAPPINGW StartOneHook(KERNEL32, "OpenFileMappingW", NewOpenFileMappingW);
		zwClose = ZWCLOSE StartOneHook(NTDLL, "ZwClose", New_ZwClose);
		unmapViewOfFile = UNMAPVIEWOFFILE StartOneHook(KERNELBASE, "UnmapViewOfFile", NewUnmapViewOfFile);
		closeHandle = CLOSEHANDLE StartOneHook(KERNEL32, "CloseHandle", NewCloseHandle);
		ntClose = NTCLOSE StartOneHook(NTDLL, "NtClose", NewNtClose);
		setFilePointer = SETFILEPOINTER StartOneHook(KERNELBASE, "SetFilePointer", NewSetFilePointer);
		getFileInformationByHandle = GETFILEINFBYHANDLE StartOneHook(KERNELBASE, "GetFileInformationByHandle", NewGetFileInformationByHandle);
		getFileSizeEx = GETFILESIZEEX StartOneHook(KERNEL32, "GetFileSizeEx", NewGetFileSizeEx);
		setFilePointerEx = SETFILEPOINTER_EX StartOneHook(KERNEL32, "SetFilePointerEx", NewSetFilePointerEx);
		getFileSize = GETFILESIZE StartOneHook(KERNEL32, "GetFileSize", NewGetFileSize);
		getSaveFileNameW = GETSAVEFILENAMEW StartOneHook(COMDLG32, "GetSaveFileNameA", New_GetSaveFileNameW);
		getFileAttributesW = GETFILEATTRIBUTESW StartOneHook(KERNELBASE, "GetFileAttributesW", NewGetFileAttributesW);
		readFileEx = READFILEEX StartOneHook(KERNEL32, "ReadFileEx", NewReadFileEx);
		readFileScatter = READFILESCATTER StartOneHook(KERNEL32, "ReadFileScatter", NewReadFileScatter);
		getFileAttributesExW = GETFILEATTRIBUTESEXW StartOneHook(KERNEL32, "GetFileAttributesEx", NewGetFileAttributesExW);

		createProcessW = CREATEPROCESS StartOneHook(KERNEL32, "CreateProcessW", NewCreateProcessW);
		createProcessInternalW = PROCESSINTERNALW StartOneHook(KERNEL32, "CreateProcessInternalW", NewCreateProcessInternal);
		//listenParentProcess(GetCurrentProcessId());
	//}
	//else
	//{
	//	bitBlt = BITBLT StartOneHook(GDI32, "BitBlt", New_BitBlt);
	//	stretchBlt = STRECTCHBLT StartOneHook(GDI32, "StretchBlt", New_StretchBlt);
	//	createProcessW = CREATEPROCESS StartOneHook(KERNEL32, "CreateProcessW", NewCreateProcessW);
	//	createProcessInternalW = PROCESSINTERNALW StartOneHook(KERNEL32, "CreateProcessInternalW", NewCreateProcessInternal);
	//}

	

}
/***********************************************
Function: StartHook
Description: ֹͣ���е�HOOK
************************************************/
void __stdcall EndHook()
{
	//if (GetParentProcessID(GetCurrentProcessId()) == *(int*)pBuffer)
	//{
		//TODO:���ж��ļ��Ľ�β��Ϣ�ķ����Ӷ�ȥ������Ӧ��HOOK����
		EndOneHook(USER32, openClipboard, NewOpenClipboard);
		EndOneHook(WINSPOOL, openPrinter, NewOpenPrinter);
		EndOneHook(USER32, showWindow, NewShowWindow);
		EndOneHook(USER32, setWindowPos, NewSetWindowPos);
		EndOneHook(USER32, setWindowTextW, NewSetWindowTextW);
		EndOneHook(USER32, setWindowTextA, NewSetWindowTextA);
		EndOneHook(KERNEL32, createFileW, NewCreateFileW);
		EndOneHook(KERNEL32, readfile, NewReadfile);
		EndOneHook(KERNEL32, writeFile, New_WriteFile);
		EndOneHook(KERNEL32, mapViewOfFile, NewMapViewOfFile);
		EndOneHook(KERNEL32, mapViewOfFileEx, NewMapViewOfFileEx);
		EndOneHook(KERNEL32, createFileMapping, NewCreateFileMapping);
		EndOneHook(KERNEL32, openFileMappingW, NewOpenFileMappingW);
		EndOneHook(NTDLL, zwClose, New_ZwClose);
		EndOneHook(KERNELBASE, unmapViewOfFile, NewUnmapViewOfFile);
		EndOneHook(KERNEL32, closeHandle, NewCloseHandle);
		EndOneHook(NTDLL, ntClose, NewNtClose);
		EndOneHook(KERNELBASE, setFilePointer, NewSetFilePointer);
		EndOneHook(KERNELBASE, getFileInformationByHandle, NewGetFileInformationByHandle);
		EndOneHook(KERNEL32, getFileSizeEx, NewGetFileSizeEx);
		EndOneHook(KERNEL32, setFilePointerEx, NewSetFilePointerEx);
		EndOneHook(KERNEL32, getFileSize, NewGetFileSize);
		EndOneHook(COMDLG32, getSaveFileNameW, New_GetSaveFileNameW);
		EndOneHook(KERNELBASE, getFileAttributesW, NewGetFileAttributesW);
		EndOneHook(KERNEL32, readFileEx, NewReadFileEx);
		EndOneHook(KERNEL32, ReadFileScatter, NewReadFileScatter);
		EndOneHook(KERNELBASE, getFileAttributesExW, NewGetFileAttributesExW);
		EndOneHook(KERNEL32, createProcessW, NewCreateProcessW);
		EndOneHook(KERNEL32, createProcessInternalW, NewCreateProcessInternal);
		//EndOneHook(KERNEL32, createProcessInternalW, NewCreateProcessInternal);
		TerminateProcess(GetCurrentProcess(), 0);
	//}
	//else
	//{
	//	EndOneHook(GDI32, bitBlt, New_BitBlt);
	//	EndOneHook(GDI32, stretchBlt, New_StretchBlt);
	//	EndOneHook(KERNEL32, createProcessW, NewCreateProcessW);
	//	EndOneHook(KERNEL32, createProcessInternalW, NewCreateProcessInternal);
	//}
}











