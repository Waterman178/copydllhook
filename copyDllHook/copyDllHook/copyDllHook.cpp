// copyDllHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "copyDllHook.h"
#include "DebugPrintf.h"
#include "GetParentProID.h"
#include "GlobalHook.h"
#include "../../OutGoingFileTool/OutGoingFileTool/FIlestruct.h"

#define FILE_SIGN "DSKFJLSKDF"
#define FileName "RjiSafe9575"
#define FILE_SIGN_LEN 11

  NTSTATUS(NTAPI* orgZwCreateSection)(__out PHANDLE SectionHandle, __in ACCESS_MASK DesiredAccess, __in_opt POBJECT_ATTRIBUTES ObjectAttributes, __in_opt PLARGE_INTEGER MaximumSize, __in ULONG SectionPageProtection, __in ULONG AllocationAttributes, __in_opt HANDLE FileHandle);
  NTSTATUS(WINAPI*  m_pfnOriginalZwReadFile) (HANDLE FileHandle, HANDLE  Event, PIO_APC_ROUTINE  ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID  Buffer, ULONG Length, PLARGE_INTEGER  ByteOffset, PULONG  Key);
 std::list<HANDLE> MAPHAD_list;
 std::list<HANDLE>::iterator map_ite;


 std::list<HANDLE> MAPHADD_list;
 std::list<HANDLE>::iterator mapp_ite;


  std::list<FileHandleRelationNode> m_handleList;
  std::list<FileHandleRelationNode>::iterator handleListNode;

 BOOL  fristopen = FALSE;

zwQueryInformationFile m_pfnOriginalZwQueryInformationFile;
myZwCreateSection    m_pfnOriginalZwCreateSection;
pfZwMapViewOfSection  m_pfnOriginalZwMapViewOfSection;
pfZwClose m_pfnOriginalZwClose;
pfmyRtlInitUnicodeString m_pfnOriginalRtlInitUnicodeString;
pfZwUnmapViewOfSection  m_pfnOriginalZwUnmapViewOfSection;
pfnOriginalZwSetInformationFile m_pfnOriginalZwSetInformationFile;


//全局变量

HANDLE  dochFile = (HANDLE)-1;//word的加密文件的句柄

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
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>被HOOK的函数的新功能<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
/***********************************************
OpenClipboard打开剪切板
************************************************/
//static BOOL WINAPI NewOpenClipboard(
//	_In_opt_ HWND hWndNewOwner
//	)
//{
//	if (GetParentProcessID(GetCurrentProcessId()) == *(int*)pBuffer)
//	{
//		return FALSE;
//	}
//	return openClipboard(hWndNewOwner);
//	
//}

/***********************************************
GetClipboardData()函数获取剪切板的内容
************************************************/
//static HANDLE WINAPI NewGetClipboardData(
//	_In_ UINT uFormat
//	)
//{
//	if (GetParentProcessID(GetCurrentProcessId()) == *(int*)pBuffer)
//	{
//		return NULL;
//	}
//	return getClipboardData(uFormat);
//}

/***********************************************
OpenPrinter()函数 打印功能
************************************************/
//static BOOL WINAPI NewOpenPrinter(__in LPTSTR pPrinterName,
//	__out LPHANDLE phPrinter,
//	__in LPPRINTER_DEFAULTS pDefault
//	)
//{
//	if (GetParentProcessID(GetCurrentProcessId()) == *(int*)pBuffer)
//	{
//		return FALSE;
//	}
//	return openPrinter(pPrinterName, phPrinter, pDefault);
//}

/***********************************************
showWindow()函数 另存为功能
************************************************/
static BOOL WINAPI NewShowWindow(HWND hWnd, int nCmdShow)
{
	//CopyFileExW
	//OutputDebugStringEx("HOOK  showWindow: hwnd = %d,nCmdShow = %d", hWnd, nCmdShow);
	return showWindow(hWnd, nCmdShow);
}
/***********************************************
SetWindowPos()函数 改变一个子窗口，
弹出式窗口或顶层窗口的尺寸，位置和Z序功能
************************************************/
static BOOL WINAPI NewSetWindowPos(_In_     HWND hWnd,//在z序中的位于被置位的窗口前的窗口句柄。该参数必须为一个窗口句柄
	_In_opt_ HWND hWndInsertAfter,//用于标识在z-顺序的此 CWnd 对象之前的 CWnd 对象
	_In_     int  X,//以客户坐标指定窗口新位置的左边界。
	_In_     int  Y,//以客户坐标指定窗口新位置的顶边界。
	_In_     int  cx,//以像素指定窗口的新的宽度。
	_In_     int  cy,//以像素指定窗口的新的高度。
	_In_     UINT uFlags//窗口尺寸和定位的标志,15种参数
	)
{
	
	//OutputDebugStringEx("HOOK  setWindow: hwnd = %d,nCmdShow = %d ,uFlag = %d", hWnd, hWndInsertAfter, uFlags);
	return setWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}
/***********************************************
SetWindowTextW()函数  SetWindowTextA()函数 
设置对话框标题或者对话框控件文本的内容
************************************************/
static BOOL WINAPI NewSetWindowTextW(HWND hwnd, //要改变文本内容的窗口或控件的句柄
	LPCTSTR lpString)//指向一个空结束的字符串的指针，该字符串将作为窗口或控件的新文本
{
	//OutputDebugStringEx("HOOK  setWindowW: hwnd = %d,nCmdShow = %s", hwnd, lpString);
	//CString otherSave = _T("另存为");
	//if (0 == memcmp(otherSave.GetBuffer(), lpString, lstrlenW(lpString)))
	//{
	//	//OutputDebugStringEx("测试:return False>>>另存为 ");
	//	//CloseWindow(hwnd);
	//	SendMessage(hwnd, WM_CLOSE, 0, 0);
	//}
	//otherSave = _T("发布为 PDF 或 XPS");
	//if (0 == memcmp(otherSave.GetBuffer(), lpString, lstrlenW(lpString)))
	//{
	//	//OutputDebugStringEx("测试:return False>>>另存为 ");
	//	//CloseWindow(hwnd);
	//	SendMessage(hwnd, WM_CLOSE, 0, 0);
	//}
	//otherSave = _T("保存(&S)");
	//if (0 == memcmp(otherSave.GetBuffer(), lpString, lstrlenW(lpString)))
	//{
	//	//OutputDebugStringEx("HOOK测试:return False>>>保存 ");
	//	//CloseWindow(hwnd);
	//	return setWindowTextW(hwnd, NULL);
	//}
	return setWindowTextW(hwnd, lpString);
}

static BOOL WINAPI NewSetWindowTextA(HWND hwnd, //要改变文本内容的窗口或控件的句柄
	LPCTSTR lpString)//指向一个空结束的字符串的指针，该字符串将作为窗口或控件的新文本
{
	//OutputDebugStringEx("HOOK  setWindowA: hwnd = %d,nCmdShow = %s", hwnd, lpString);
	return setWindowTextA(hwnd, lpString);
}

/***********************************************
CreateFileW()函数			函数创建或打开下列对象
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
	HANDLE keyHan = nullptr;
	BOOL bReadDecrypt = FALSE;
	if (memcmp(lpFileName, _T("\\\\"), 4) != 0 && wcswcs((wchar_t*)lpFileName,L".docx")!=NULL ) {
		keyHan = createFileW(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		/*OutputDebugStringEx(">>>>>>>>HOOK THE NewCreateFileW %d %ws\r\n", keyHan, lpFileName);
		if (!m_handleList.empty())
		{
			for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
			{
				if (handleListNode->FileHandle == keyHan)
				{
					handleListNode->m_FileInfo.bReadDecrypt = FALSE;
					return keyHan;
				}
			}
		}*/
		if (keyHan != INVALID_HANDLE_VALUE) {
			DWORD readLen;
			LPVOID fileHead = new char[FILE_SIGN_LEN];
			int currentPointer = 0;
			OutputDebugStringEx("ReadFile Begin");
			currentPointer = SetFilePointer(keyHan, 0, NULL, FILE_BEGIN);
			//setFilePointer(keyHan, -FILE_SIGN_LEN, NULL, FILE_END);
			OutputDebugStringEx(">>>>>>>>readfile<<<<<<<<\r\n");
			ReadFile(keyHan, fileHead, FILE_SIGN_LEN, &readLen, NULL);
			SetFilePointer(keyHan, 0, NULL, FILE_BEGIN);
			OutputDebugStringEx("******HOOK: fileHead = %s\r\n", fileHead);
			//closeHandle(keyHan);
			CLOSEHANDLE(keyHan);
			if (memcmp(fileHead, FileName, FILE_SIGN_LEN) == 0)
			{
				HANDLE ret = createFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
				//SetFilePointer(ret, sizeof(RjFileSrtuct), NULL, FILE_BEGIN);
				//MAPHAD_list.push_back(ret);

				auto pRobj = new FileHandleRelationNode;
				pRobj->FileHandle = ret;
				pRobj->m_FileInfo.bReadDecrypt = TRUE;
				pRobj->m_FileInfo.bEncryptFile = TRUE;
				m_handleList.push_back(*pRobj);
				delete fileHead;
				return ret;
				/*OutputDebugStringEx("**************HOOK:sercret file\r\n ");*/
				/*auto Attribute = GetFileAttributesA(reinterpret_cast<LPCSTR>(lpFileName))&FILE_ATTRIBUTE_READONLY;
				if (Attribute!= FILE_ATTRIBUTE_READONLY)
				{
					OutputDebugStringEx("不是只读文件，设置只读属性\r\n ");
					SetFileAttributesA(reinterpret_cast<LPCSTR>(lpFileName), FILE_ATTRIBUTE_READONLY);
				}*/

				//dwDesiredAccess &= ~GENERIC_WRITE;
			}
			//else if (bReadDecrypt == TRUE)
			//{
			//	OutputDebugStringEx("无需解密\r\n ");
			//	//handleListNode->m_FileInfo.bReadDecrypt = FALSE;
			//	return keyHan;
			//}
		}
	}
	HANDLE ret = createFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	
	//OutputDebugStringEx(">>>>>>>>HOOK THE NewCreateFileW: %s  ,  %d\r\n", lpFileName, ret);
	return ret;
}

/***********************************************
Readfile()函数			函数读取文件功能   读取文件不是这么写，这里作废
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
	int HeadFlag = sizeof(RjFileSrtuct);
	DWORD readLen;
	LPVOID fileHead = new char[HeadFlag];
	int currentPointer = 0;

	setFilePointer(hFile, -HeadFlag, NULL, FILE_END);
	readfile(hFile, fileHead, HeadFlag, &readLen, lpOverlapped);
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

		//TODO:针对最后的添加信息要进行处理一下

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
Weadfile()函数			函数写文件功能
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
MapViewOfFile()函数			文件映射对象映射到当前应用程序的地址空间功能
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

	LPVOID pMsg;
	if (MAPHADD_list.empty())
	{
		pMsg = mapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, 10);
		return pMsg;
	}
	else
	{
		for (mapp_ite = MAPHADD_list.begin(); mapp_ite != MAPHAD_list.end(); mapp_ite++)
		{
			if (hFileMappingObject == *mapp_ite)
			{
				//dwNumberOfBytesToMap -= sizeof(RjFileSrtuct);
				//dwNumberOfBytesToMap --;
				dwNumberOfBytesToMap -= sizeof(RjFileSrtuct);
				OutputDebugStringEx(">>>>>>>>HOOK THE MapViewOfFile dwNumberOfBytesToMap %d \r\n", dwNumberOfBytesToMap);
				pMsg = mapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap);
				return pMsg;
			}
		}
		pMsg = mapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, 10);
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
CreateFileMapping()函数			创建一个新的文件映射内核对象功能
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

	HANDLE fileMap;
	DWORD readLen;
	int HeadFlaglength = sizeof(FILE_SIGN_LEN);
	char* fileHead = new char[FILE_SIGN_LEN];
	//int currentPointer = 0;
	//currentPointer = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	//SetFilePointer(hFile, -HeadFlaglength, NULL, FILE_END);
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	ReadFile(hFile, fileHead, HeadFlaglength, &readLen, NULL); //用原始的
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	OutputDebugStringEx("******HOOK: fileHead = %s", fileHead);
	if (memcmp(fileHead, FileName, FILE_SIGN_LEN) == 0) //这里改成我们的文件头
	{
		SetFilePointer(hFile, HeadFlaglength, NULL, FILE_BEGIN);
		//删除文件头
		//LPSYSTEM_INFO  sysinfo = new SYSTEM_INFO;
		//这里设置偏移
		/*GetSystemInfo(sysinfo);
		auto  dwAllocationGranularity = sysinfo->dwAllocationGranularity;*/
		MAPHAD_list.push_back(hFile);


		//ReleaseMutex(hMutex);
		OutputDebugStringEx("dwMaximumSizeHigh:%d dwMaximumSizeLow:%d \r\n", dwMaximumSizeHigh, dwMaximumSizeLow);
		//dwMaximumSizeLow -= sizeof(RjFileSrtuct);
		fileMap = createFileMapping(hFile, lpAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
		MAPHADD_list.push_back(fileMap);
		//WaitForSingleObject(hMutex, INFINITE);
		//ReleaseMutex(hMutex);
		fristopen = TRUE;
		OutputDebugStringEx("**************HOOK:sercret file \r\n");
		//delete sysinfo;
	}
	else
	{
		fileMap = createFileMapping(hFile, lpAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
	}
	delete fileHead;
	return fileMap;
}
/***********************************************
OpenFileMappingW()函数			打开一个新的文件映射内核对象功能
************************************************/
static HANDLE WINAPI NewOpenFileMappingW(
	_In_ DWORD   dwDesiredAccess,
	_In_ BOOL    bInheritHandle,
	_In_ LPCTSTR lpName)
{
	OutputDebugStringEx(">>>>>>>>HOOK THE OpenFileMappingW\r\n");
	OutputDebugStringEx(">HOOK< dwDesiredAccess = %d,bInheritHandle=%d,lpName=%ws\r\n", dwDesiredAccess, bInheritHandle, lpName);
	return openFileMappingW(dwDesiredAccess, bInheritHandle, lpName);
}
/***********************************************
ZwClose()函数			根据句柄关闭功能
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
SetFilePointer()函数			根据句柄关闭功能
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
GetFileInformationByHandle()函数			根据句柄关闭功能
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
GetFileAttributesW()函数			获取文件属性
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
ReadFileEx()函数			读取文件属性
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
ReadFileScatter()函数			读取文件属性
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
GetFileAttributesExW()函数			读取文件属性
************************************************/
//static DWORD WINAPI NewGetFileAttributesExW(
//	__in LPCTSTR lpFileName
//	)
//{
//	OutputDebugStringEx("HOOK:  THE NewGetFileAttributesExW FUNCTION !!\r\n");
//	DWORD result = getFileAttributesExW(lpFileName);
//	return result;
//}
/***********************************************
GetFileSizeEx()函数			功能
************************************************/
static BOOL WINAPI NewGetFileSizeEx(
	_In_  HANDLE         hFile,
	_Out_ PLARGE_INTEGER lpFileSize)
{
	BOOL result = getFileSizeEx(hFile, lpFileSize);
	int HeadFlaglength = sizeof(RjFileSrtuct);
	DWORD readLen;
	LPVOID fileHead = new char[HeadFlaglength];
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	ReadFile(hFile, fileHead, HeadFlaglength, &readLen, NULL); //用原始的
	OutputDebugStringEx("HOOK:  THE NewGetFileSizeEx FUNCTION !!    readHead = %s\r\n", fileHead);
	if (memcmp(fileHead, FileName, FILE_SIGN_LEN) == 0)
	{		
		OutputDebugStringEx("********  getFileSizeEx ******HOOK:sercret file \r\n");	
		lpFileSize->QuadPart -= HeadFlaglength;
	}
	OutputDebugStringEx("HOOK:  THE NewGetFileSizeEx FUNCTION !!  %ld\r\n ", lpFileSize->QuadPart);
	delete fileHead;
	return result;
}
/***********************************************
SetFilePointerEx()函数			功能
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
		liDistanceToMove.QuadPart -= sizeof(RjFileSrtuct);
	}
	return setFilePointerEx(hFile, liDistanceToMove, lpNewFilePointer, dwMoveMethod);
}
/***********************************************
GetFileSize()函数			A获取文件长度功能
************************************************/
static DWORD WINAPI NewGetFileSize(
	HANDLE hFile,
	LPDWORD lpFileSizeHigh)
{
	DWORD result = getFileSize(hFile, lpFileSizeHigh);
	int HeadFlaglength = sizeof(RjFileSrtuct)+1;
	DWORD readLen;
	LPVOID fileHead = new char[HeadFlaglength];
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	ReadFile(hFile, fileHead, HeadFlaglength, &readLen, NULL); //用原始的
	//OutputDebugStringEx("HOOK:  THE NewGetFileSize FUNCTION !!    readHead = %s\r\n", fileHead);
	if (memcmp(fileHead, FileName, FILE_SIGN_LEN) == 0)
	{
		//OutputDebugStringEx("********  getFileSize ******HOOK:sercret file \r\n");
		result -= HeadFlaglength;
	}
	return result;
}
/***********************************************
GetSaveFileNameW()函数			A获取关闭文件会话框
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
UnmapViewOfFile()函数			停止内存映射功能
************************************************/
static BOOL WINAPI NewUnmapViewOfFile(_In_ LPCVOID lpBaseAddress)
{
	return unmapViewOfFile(lpBaseAddress);
}
/***********************************************
BitBlt()函数			函数截图功能
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
StretchBlt()函数			函数截图功能
************************************************/
static BOOL WINAPI New_StretchBlt(HDC hdcDest, int nXOriginDest,
	int nYOriginDest, int nWidthDest,
	int nHeightDest, HDC hdcSrc,
	int nXOriginSrc, int nYOriginSrc, int nWidthSrc,
	int nHeightSrc, DWORD dwRop)
{
	//OutputDebugStringEx("HOOK测试2New_StretchBlt： hdcDest = %d,hdcSrc = %d,dwRop = %d", hdcDest, hdcSrc, dwRop);
	return stretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop);
}

/***********************************************
CreateProcessW()函数			创建进程功能
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

	//InjectDll(lpProcessInformation->dwProcessId, _T("E:\\外发工具\\HookProject_test\\fileHook\\OutGoingFileTool\\Debug\\copyDllHook.dll"));
		
	return result;
}
/***********************************************
CreateProcessInternal()函数			创建进程功能
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
	InjectDll(lpProcessInformation->dwProcessId, _T("E:\\外发工具\\HookProject_test\\fileHook\\OutGoingFileTool\\Debug\\copyDllHook.dll\r\n"));

	return result;
}



static
HANDLE
WINAPI
NewCreateFileMappingA(
	__in     HANDLE hFile,
	__in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
	__in     DWORD flProtect,
	__in     DWORD dwMaximumSizeHigh,
	__in      DWORD dwMaximumSizeLow,
	__in_opt LPCSTR lpName
) {
	OutputDebugStringEx("-----HOOK---createfilemappingA完成");
	return createfilemappingA(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>HOOK功能集函数<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/***********************************************

************************************************/
PVOID FindProcAddress(_In_ LPCTSTR lpFileName, LPCSTR lpProcName)
{
	PVOID insthook;
	HINSTANCE hinst = NULL;
	hinst = LoadLibrary(lpFileName);//加载dll文件
	if (hinst == NULL)
	{
		return NULL;
	}
	insthook = GetProcAddress(hinst, lpProcName);//获取函数地址
	if (insthook == NULL)
	{
		return NULL;
	}
	return insthook;
}
/***********************************************
Function: StartOneHook

Description: 开始HOOK一个函数

Input:	LPCTSTR dllName 函数所在的dll库
		LPCSTR funcName 函数名称
		PVOID newFunc	新函数的函数指针
Returns: 成功：旧函数地址 失败：NULL
************************************************/
PVOID StartOneHook(LPCTSTR dllName, LPCSTR funcName,PVOID newFunc)
{
	//开始事务
	DetourTransactionBegin();
	//更新线程信息  
	DetourUpdateThread(GetCurrentThread());
	PVOID oldFunc = FindProcAddress(dllName,funcName);
	if (oldFunc == NULL)
	{
		return NULL;
	}
	//将拦截的函数附加到原函数的地址上
	DetourAttach(&oldFunc, newFunc);
	//结束事务
	DetourTransactionCommit();
	return oldFunc;
}
/***********************************************
Function: EndOneHook

Description: 停止HOOK一个函数

Input:	LPCTSTR dllName 函数所在的dll库
		LPCSTR funcName 函数名称
		PVOID newFunc	新函数的函数指针

************************************************/
void EndOneHook(LPCTSTR dllName, PVOID oldFunc, PVOID newFunc)
{
	////OutputDebugStringEx("end the funcName=%s ", funcName);
	//开始事务
	DetourTransactionBegin();
	//更新线程信息 
	DetourUpdateThread(GetCurrentThread());
	//将拦截的函数从原函数的地址上解除
	//PVOID oldFunc = FindProcAddress(dllName,funcName);
	//if (oldFunc == NULL)
	//{
	//	return;
	//}
	DetourDetach(&oldFunc, newFunc);
	//结束事务
	DetourTransactionCommit();
}
/***********************************************
Function: rc4

Description: rc4加密算法

Input:	char *pSecret KEY
		int SecretLen 密钥长度
		char *pMessage 加密信息
		int MessageLen 加密信息长度
		char *pOut 输出加密后的信息
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
BOOL  HOOKGetFileAttributesExW(
	LPCWSTR                lpFileName,
	GET_FILEEX_INFO_LEVELS fInfoLevelId,
	WIN32_FILE_ATTRIBUTE_DATA       *          lpFileInformation
) {
	OutputDebugStringEx("  .............. HOOKGetFileAttributesExW ");
	OutputDebugStringEx("HOOKGetFileAttributesExW open File:%ws fail", lpFileName);
	BOOL ret = FALSE;
	auto  FileHandle = CreateFileW(lpFileName, GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (FileHandle=NULL)
	{
		OutputDebugStringEx("HOOKGetFileAttributesExW open File:%ws fail", lpFileName);
	}
	int HeadFlaglength = sizeof(RjFileSrtuct);
	DWORD readLen;
	LPVOID fileHead = new char[HeadFlaglength];
	SetFilePointer(FileHandle, 0, NULL, FILE_BEGIN);
	ReadFile(FileHandle, fileHead, HeadFlaglength, &readLen, NULL); //用原始的
	//OutputDebugStringEx("HOOK:  THE NewGetFileSize FUNCTION !!    readHead = %s\r\n", fileHead);
	if (memcmp(fileHead, FileName, FILE_SIGN_LEN) == 0)
	{
		OutputDebugStringEx("HOOKGetFileAttributesExW find org File lpFileInformation->nFileSizeLow:%d", lpFileInformation->nFileSizeLow);
		ret = getFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);
		lpFileInformation->nFileSizeLow -= HeadFlaglength;
	}
	else {
		ret = getFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);
	}
	return ret;

}
BOOL  WINAPI  hookGetFileInformationByHandle(HANDLE hFile, LPBY_HANDLE_FILE_INFORMATION lpFileInformation) {
	BOOL a  = pfGetFileInformationByHandle(hFile, lpFileInformation);
	int HeadFlaglength = sizeof(RjFileSrtuct);
	DWORD readLen;
	LPVOID fileHead = new char[HeadFlaglength];
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	ReadFile(hFile, fileHead, HeadFlaglength, &readLen, NULL); //用原始的
	OutputDebugStringEx(" hookGetFileInformationByHandle");
													   //OutputDebugStringEx("HOOK:  THE NewGetFileSize FUNCTION !!    readHead = %s\r\n", fileHead);
	if (memcmp(fileHead, FileName, FILE_SIGN_LEN) == 0)
	{
		OutputDebugStringEx( " hookGetFileInformationByHandle->lpFileInformation->nFileSizeLow:%d", lpFileInformation->nFileSizeLow);
		lpFileInformation->nFileSizeLow -= HeadFlaglength;
		lpFileInformation->nFileSizeLow--;
	}
	return a;
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
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>DLL导出函数集合<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
/***********************************************
Function: StartHook

Description: 开始所有的HOOK

************************************************/
void __stdcall StartHook()
{
	/*openClipboard = OPENCLIPBOARD StartOneHook(USER32, "OpenClipboard", NewOpenClipboard);
	openPrinter = OPENPRINTER StartOneHook(WINSPOOL, "OpenPrinterW", NewOpenPrinter);
	showWindow = SHOWWINDOW StartOneHook(USER32, "ShowWindow", NewShowWindow);
	setWindowPos = SETWINDOWPOS StartOneHook(USER32, "SetWindowPos", NewSetWindowPos);
	setWindowTextW = SETWINDOWTEXTW StartOneHook(USER32, "SetWindowTextW", NewSetWindowTextW);
	setWindowTextA = SETWINDOWTEXTA StartOneHook(USER32, "SetWindowTextA", NewSetWindowTextA);*/

		m_pfnOriginalZwReadFile = ZwReadFile StartOneHook(NTDLL, "ZwReadFile", HookZwReadFile);
		if (m_pfnOriginalZwReadFile == 0x00) {
			OutputDebugStringEx("m_pfnOriginalZwReadFile获取失败");
			return;
		}

		createFileW = CREATEFILEW StartOneHook(KERNEL32, "CreateFileW", NewCreateFileW);

		//readfile = READFILE StartOneHook(KERNEL32, "ReadFile", NewReadfile);
		//writeFile = WRITEFILE StartOneHook(KERNEL32, "WriteFile", New_WriteFile);

	   // getFileAttributesExW = GetFileAttributesExW StartOneHook(KERNEL32, "GetFileAttributesExW", HOOKGetFileAttributesExW);
	    
		m_pfnOriginalZwQueryInformationFile =   (zwQueryInformationFile)FindProcAddress(NTDLL, "ZwQueryInformationFile");
		if (m_pfnOriginalZwQueryInformationFile == 0x00) {
			OutputDebugStringEx("m_pfnOriginalZwQueryInformationFile获取失败");
			return ; 
		}
		m_pfnOriginalZwCreateSection =  (myZwCreateSection)FindProcAddress(NTDLL, "ZwCreateSection");
		if (m_pfnOriginalZwCreateSection == 0x00) { 
			OutputDebugStringEx("m_pfnOriginalZwCreateSection获取失败");
			return;}
		m_pfnOriginalZwClose  = (pfZwClose)FindProcAddress(NTDLL, "ZwClose");
		if (m_pfnOriginalZwClose == 0x00) { 
			OutputDebugStringEx("m_pfnOriginalZwClose获取失败");
			return; }
	    m_pfnOriginalZwMapViewOfSection  = (pfZwMapViewOfSection)FindProcAddress(NTDLL, "ZwMapViewOfSection");
		if (m_pfnOriginalZwMapViewOfSection == 0x00) {
			OutputDebugStringEx("m_pfnOriginalZwMapViewOfSection获取失败");
			return; }
		m_pfnOriginalRtlInitUnicodeString =  (pfmyRtlInitUnicodeString)FindProcAddress(NTDLL, "RtlInitUnicodeString");
		if (m_pfnOriginalRtlInitUnicodeString == 0x00) { 
			OutputDebugStringEx("RtlInitUnicodeString获取失败");
			return; };
		m_pfnOriginalZwUnmapViewOfSection =  (pfZwUnmapViewOfSection)FindProcAddress(NTDLL, "ZwUnmapViewOfSection");
		if (m_pfnOriginalZwUnmapViewOfSection == 0x00) { 
			OutputDebugStringEx("m_pfnOriginalZwUnmapViewOfSection获取失败");
			return; };
		m_pfnOriginalZwSetInformationFile = (pfnOriginalZwSetInformationFile)FindProcAddress(NTDLL, "ZwSetInformationFile");
		if (m_pfnOriginalZwSetInformationFile == 0x00) {
			OutputDebugStringEx("m_pfnOriginalZwSetInformationFile获取失败");
			return;}

		MessageBox(NULL, "1111", "dsadsa", MH_OK);

		//createFileMapping = CREATEFILEMAPPING StartOneHook(KERNEL32, "CreateFileMappingW", NewCreateFileMapping);
		//pfGetFileInformationByHandle = GetFileInformationByHandle StartOneHook(KERNEL32, "GetFileInformationByHandle", hookGetFileInformationByHandle);
		//orgZwCreateSection = ZwCreateSection StartOneHook(NTDLL, "ZwCreateSection", HookZwCreateSection);
	


		//getFileSize = GETFILESIZE StartOneHook(KERNEL32, "GetFileSize", NewGetFileSize);





		
	   // mapViewOfFile = MAPVIEWOFFILE StartOneHook(KERNEL32, "MapViewOfFile", NewMapViewOfFile);
		//mapViewOfFileEx = MAPVIEWOFFILEEX StartOneHook(KERNEL32, "MapViewOfFileEx", NewMapViewOfFileEx);

		//createfilemappingA = CREATEFILEMAPPINGA StartOneHook(KERNEL32, "CreateFileMappingA", NewCreateFileMappingA);
		
		//openFileMappingW = OPENFILEMAPPINGW StartOneHook(KERNEL32, "OpenFileMappingW", NewOpenFileMappingW);

	/*	if (MH_CreateHook(&m_pfnOriginalZwCreateSection,&HookZwCreateSection,reinterpret_cast<void**>(&orgZwCreateSection))!=MH_OK)
		{
			OutputDebugStringEx("orgZwCreateSection获取失败");
		}*/
		/*DetourTransactionBegin();
		DetourUpdateThread(NtCurrentThread);
		DetourAttach((PVOID*)&orgZwCreateSection, HookZwCreateSection);
		DetourTransactionCommit();*/

		//m_pfnOriginalZwQueryInformationFile = ZwQueryInformationFile  StartOneHook(NTDLL, "ZwQueryInformationFile", NewOpenFileMappingW);
		/*zwClose = ZWCLOSE StartOneHook(NTDLL, "ZwClose", New_ZwClose);
		unmapViewOfFile = UNMAPVIEWOFFILE StartOneHook(KERNELBASE, "UnmapViewOfFile", NewUnmapViewOfFile);
		closeHandle = CLOSEHANDLE StartOneHook(KERNEL32, "CloseHandle", NewCloseHandle);
		ntClose = NTCLOSE StartOneHook(NTDLL, "NtClose", NewNtClose);
		setFilePointer = SETFILEPOINTER StartOneHook(KERNELBASE, "SetFilePointer", NewSetFilePointer);
		getFileInformationByHandle = GETFILEINFBYHANDLE StartOneHook(KERNELBASE, "GetFileInformationByHandle", NewGetFileInformationByHandle);
		getFileSizeEx = GETFILESIZEEX StartOneHook(KERNEL32, "GetFileSizeEx", NewGetFileSizeEx);
		setFilePointerEx = SETFILEPOINTER_EX StartOneHook(KERNEL32, "SetFilePointerEx", NewSetFilePointerEx);
		
		getSaveFileNameW = GETSAVEFILENAMEW StartOneHook(COMDLG32, "GetSaveFileNameA", New_GetSaveFileNameW);
		getFileAttributesW = GETFILEATTRIBUTESW StartOneHook(KERNELBASE, "GetFileAttributesW", NewGetFileAttributesW);
		readFileEx = READFILEEX StartOneHook(KERNEL32, "ReadFileEx", NewReadFileEx);
		readFileScatter = READFILESCATTER StartOneHook(KERNEL32, "ReadFileScatter", NewReadFileScatter);
		getFileAttributesExW = GETFILEATTRIBUTESEXW StartOneHook(KERNEL32, "GetFileAttributesEx", NewGetFileAttributesExW);

		createProcessW = CREATEPROCESS StartOneHook(KERNEL32, "CreateProcessW", NewCreateProcessW);
		createProcessInternalW = PROCESSINTERNALW StartOneHook(KERNEL32, "CreateProcessInternalW", NewCreateProcessInternal);*/
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
Description: 停止所有的HOOK
************************************************/
void __stdcall EndHook()
{
	//if (GetParentProcessID(GetCurrentProcessId()) == *(int*)pBuffer)
	//{
		//TODO:进行对文件的结尾信息的分析从而去结束对应的HOOK功能
		/*EndOneHook(USER32, openClipboard, NewOpenClipboard);
		EndOneHook(WINSPOOL, openPrinter, NewOpenPrinter);
		EndOneHook(USER32, showWindow, NewShowWindow);
		EndOneHook(USER32, setWindowPos, NewSetWindowPos);
		EndOneHook(USER32, setWindowTextW, NewSetWindowTextW);
		EndOneHook(USER32, setWindowTextA, NewSetWindowTextA);*/
		EndOneHook(KERNEL32, createFileW, NewCreateFileW);
		//EndOneHook(KERNEL32, readfile, NewReadfile);
		//EndOneHook(KERNEL32, writeFile, New_WriteFile);
		//EndOneHook(KERNEL32, mapViewOfFile, NewMapViewOfFile);
		//EndOneHook(KERNEL32, mapViewOfFileEx, NewMapViewOfFileEx);
		//EndOneHook(KERNEL32, createFileMapping, NewCreateFileMapping);
		//EndOneHook(KERNEL32, pfGetFileInformationByHandle, hookGetFileInformationByHandle);
		//EndOneHook(NTDLL, orgZwCreateSection, HookZwCreateSection);
		EndOneHook(NTDLL, m_pfnOriginalZwReadFile, HookZwReadFile);

		//EndOneHook(NTDLL, getFileAttributesExW, HOOKGetFileAttributesExW);
		//EndOneHook(KERNEL32, openFileMappingW, NewOpenFileMappingW);
		//EndOneHook(KERNEL32, getFileSizeEx, NewGetFileSizeEx);
		//EndOneHook(KERNEL32, getFileSize, NewGetFileSize);
		/*EndOneHook(NTDLL, zwClose, New_ZwClose);
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
		EndOneHook(KERNEL32, createProcessInternalW, NewCreateProcessInternal);*/
		//EndOneHook(KERNEL32, createProcessInternalW, NewCreateProcessInternal);
		//TerminateProcess(GetCurrentProcess(), 0);
	//}
	//else
	//{
	//	EndOneHook(GDI32, bitBlt, New_BitBlt);
	//	EndOneHook(GDI32, stretchBlt, New_StretchBlt);
	//	EndOneHook(KERNEL32, createProcessW, NewCreateProcessW);
	//	EndOneHook(KERNEL32, createProcessInternalW, NewCreateProcessInternal);
	//}
}














