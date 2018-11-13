// copyDllHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "copyDllHook.h"
#include "DebugPrintf.h"
#include "GetParentProID.h"
#include "GlobalHook.h"

#define FILE_SIGN "DSKFJLSKDF"
#define FILE_SIGN_LEN 10

//全局变量
std::list<HANDLE> MAPHAD_list;
std::list<HANDLE>::iterator map_ite;
HANDLE  dochFile = (HANDLE)-1;//word的加密文件的句柄

HANDLE hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, _T("processMem_FUCK"));
PVOID pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);


/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>被HOOK的函数的新功能<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
/***********************************************
OpenClipboard打开剪切板
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
GetClipboardData()函数获取剪切板的内容
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
OpenPrinter()函数 打印功能
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
showWindow()函数 另存为功能
************************************************/
static BOOL WINAPI NewShowWindow(HWND hWnd, int nCmdShow)
{
	//CopyFileExW
	//DP2("HOOK  showWindow: hwnd = %d,nCmdShow = %d", hWnd, nCmdShow);
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
	
	//DP3("HOOK  setWindow: hwnd = %d,nCmdShow = %d ,uFlag = %d", hWnd, hWndInsertAfter, uFlags);
	return setWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}
/***********************************************
SetWindowTextW()函数  SetWindowTextA()函数 
设置对话框标题或者对话框控件文本的内容
************************************************/
static BOOL WINAPI NewSetWindowTextW(HWND hwnd, //要改变文本内容的窗口或控件的句柄
	LPCTSTR lpString)//指向一个空结束的字符串的指针，该字符串将作为窗口或控件的新文本
{
	//DP2("HOOK  setWindowW: hwnd = %d,nCmdShow = %s", hwnd, lpString);
	CString otherSave = _T("另存为");
	if (0 == memcmp(otherSave.GetBuffer(), lpString, lstrlenW(lpString)))
	{
		//DP0("测试:return False>>>另存为 ");
		//CloseWindow(hwnd);
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}
	otherSave = _T("发布为 PDF 或 XPS");
	if (0 == memcmp(otherSave.GetBuffer(), lpString, lstrlenW(lpString)))
	{
		//DP0("测试:return False>>>另存为 ");
		//CloseWindow(hwnd);
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}
	otherSave = _T("保存(&S)");
	if (0 == memcmp(otherSave.GetBuffer(), lpString, lstrlenW(lpString)))
	{
		//DP0("HOOK测试:return False>>>保存 ");
		//CloseWindow(hwnd);
		return setWindowTextW(hwnd, NULL);
	}
	return setWindowTextW(hwnd, lpString);
}

static BOOL WINAPI NewSetWindowTextA(HWND hwnd, //要改变文本内容的窗口或控件的句柄
	LPCTSTR lpString)//指向一个空结束的字符串的指针，该字符串将作为窗口或控件的新文本
{
	//DP2("HOOK  setWindowA: hwnd = %d,nCmdShow = %s", hwnd, lpString);
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

	if (memcmp(lpFileName, _T("\\\\"), 4) != 0) {
		HANDLE keyHan = createFileW(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		//DP2(">>>>>>>>HOOK THE NewCreateFileW %d %s", keyHan, lpFileName);
		if (keyHan != INVALID_HANDLE_VALUE) {
			DWORD readLen;
			LPVOID fileHead = new char[FILE_SIGN_LEN];
			int currentPointer = 0;
			//currentPointer = setFilePointer(keyHan, 0, NULL, FILE_CURRENT);
			setFilePointer(keyHan, -FILE_SIGN_LEN, NULL, FILE_END);
			readfile(keyHan, fileHead, FILE_SIGN_LEN, &readLen, NULL);
			/*setFilePointer(keyHan, currentPointer, NULL, FILE_BEGIN)*/;
			//DP1("******HOOK: fileHead = %s", fileHead);
			closeHandle(keyHan);
			if (memcmp(fileHead, FILE_SIGN, FILE_SIGN_LEN) == 0)
			{
				DP0("**************HOOK:sercret file ");
				dwDesiredAccess &= ~GENERIC_WRITE;
			}
			delete fileHead;
		}
	}
	HANDLE ret = createFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	
	DP2(">>>>>>>>HOOK THE NewCreateFileW: %s  ,  %d", lpFileName, ret);
	return ret;
}

/***********************************************
Readfile()函数			函数读取文件功能
************************************************/
static int WINAPI NewReadfile(
	_In_        HANDLE       hFile,
	_Out_       LPVOID       lpBuffer,
	_In_        DWORD        nNumberOfBytesToRead,
	_Out_opt_   LPDWORD      lpNumberOfBytesRead,
	_Inout_opt_ LPOVERLAPPED lpOverlapped)
{
	DP0(">>>>>>>HOOK:   readfile function !!!");
	int Ret = 0;
	DWORD readLen;
	LPVOID fileHead = new char[FILE_SIGN_LEN];
	int currentPointer = 0;
	currentPointer = setFilePointer(hFile, 0, NULL, FILE_CURRENT);
	setFilePointer(hFile, -FILE_SIGN_LEN, NULL, FILE_END);
	readfile(hFile, fileHead, FILE_SIGN_LEN, &readLen, lpOverlapped);
	setFilePointer(hFile, currentPointer, NULL, FILE_BEGIN);
	DP3("$$$$$$$$$HOOK : THE FILE currentPointer = %d,   nNumberOfBytesToRead=%d,getfilesize=%d", currentPointer, nNumberOfBytesToRead, getFileSize(hFile, NULL));
	if (memcmp(fileHead, FILE_SIGN, FILE_SIGN_LEN) == 0)
	{
		dochFile = hFile;
		if (currentPointer + nNumberOfBytesToRead > NewGetFileSize(hFile, NULL))
		{

			DP0(">>>>>>>HOOK:  currentPointer + nNumberOfBytesToRead > getFileSize(hFile, NULL) - 10!!!");
			DWORD  temp = NewGetFileSize(hFile, NULL) - currentPointer;
			if (temp > 0)
			{
				nNumberOfBytesToRead = temp;
			}
		}
		
		DP2("$$$$$********$$$$$ HOOK!!!!!currentPointer = %d,nNumberOfBytesToRead=%d", currentPointer, nNumberOfBytesToRead);
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
	DP1(">>>>>>>HOOK:::  Readfile  Ret = %d", Ret);
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
	//DP2(">>>>>>>>HOOK THE WRITEFILE hFile=%d,dochFile=%d", hFile, dochFile);

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

	DP1(">>>>>>>>HOOK THE MapViewOfFile buf %d ", hFileMappingObject);

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
				DP1("@@@@@@@@HOOK: THIS IS A SECRET FILE!!!!  dwNumberOfBytesToMap = %d", dwNumberOfBytesToMap);
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

	//DP1(">>>>>>>>HOOK THE MapViewOfFileEx buf = %s",buf);
//	DP3("hFileMappingObject = %d,dwDesiredAccess=%d,dwFileOffsetHigh=%d", hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh);
//	DP3("dwFileOffsetLow = %d,dwNumberOfBytesToMap=%d,lpBaseAddress=%d", dwFileOffsetLow, dwNumberOfBytesToMap, lpBaseAddress);
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
	DP1(">>>>>>>>HOOK THE CreateFileMapping %d", hFile);
	HANDLE fileMap = createFileMapping(hFile, lpAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
	DWORD readLen;
	LPVOID fileHead = new char[FILE_SIGN_LEN];
	int currentPointer = 0;
	currentPointer = setFilePointer(hFile, 0, NULL, FILE_CURRENT);
	setFilePointer(hFile, -FILE_SIGN_LEN, NULL, FILE_END);
	readfile(hFile, fileHead, FILE_SIGN_LEN, &readLen, NULL);
	setFilePointer(hFile, currentPointer, NULL, FILE_BEGIN);
	//DP1("******HOOK: fileHead = %s", fileHead);
	if (memcmp(fileHead, FILE_SIGN, FILE_SIGN_LEN) == 0)
	{
		DP0("**************HOOK:sercret file ");
		MAPHAD_list.push_back(fileMap);
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
	DP0(">>>>>>>>HOOK THE OpenFileMappingW");
	DP3(">HOOK< dwDesiredAccess = %d,bInheritHandle=%d,lpName=%s", dwDesiredAccess, bInheritHandle, lpName);
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
	DP1("$$$$$>>>>>   3   HOOK:  THE SetFilePointer function !!!!,lDistanceToMove = %d", lpDistanceToMoveHigh);
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
	DP1("@@@@@@@@@@HOOK:  THE GETFILEINFORMATIONBYHANDLE FUNCTION !!     %d",lpFileInformation->dwFileAttributes);
	
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

	//DP2("@@@@@@@@@HOOK: GetFileAttributesW FUNCTION lpFileName=%s Attrib=%d", lpFileName, Ret);
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
	DP0("HOOK:  THE NewReadFileEx FUNCTION !!");
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
	DP0("HOOK:  THE NewReadFileScatter FUNCTION !!");
	BOOL result = readFileScatter(hFile, aSegmentArray, nNumberOfBytesToRead, lpReserved, lpOverlapped);
	return result;
}
/***********************************************
GetFileAttributesExW()函数			读取文件属性
************************************************/
static DWORD WINAPI NewGetFileAttributesExW(
	__in LPCTSTR lpFileName
	)
{
	DP0("HOOK:  THE NewGetFileAttributesExW FUNCTION !!");
	DWORD result = getFileAttributesExW(lpFileName);
	return result;
}
/***********************************************
GetFileSizeEx()函数			功能
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
	DP1("HOOK:  THE NewGetFileSizeEx FUNCTION !!    readHead = %s", fileHead);
	if (memcmp(fileHead, FILE_SIGN, FILE_SIGN_LEN) == 0)
	{		
		DP0("********  getFileSizeEx ******HOOK:sercret file ");	
		lpFileSize->QuadPart -= FILE_SIGN_LEN;
	}
	DP1("HOOK:  THE NewGetFileSizeEx FUNCTION !!  %ld ", lpFileSize->QuadPart);
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
	DP2("HOOK: THE SetFilePointerEx FUNCTION!!!!hFile=%d, liDistanceToMove=%d", hFile, liDistanceToMove);
	if (dwMoveMethod == FILE_END && hFile == dochFile)
	{
		liDistanceToMove.QuadPart -= FILE_SIGN_LEN;
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
	DP0(">>>>>>>>>>>>>>>>>>>HOOK : THE Getfilesize Function!!!!")
	if (hFile == dochFile)
	{
		DP0("********  NewGetFileSize ******HOOK:sercret file ");
		return getFileSize(hFile, NULL) - FILE_SIGN_LEN;
	}
	return getFileSize(hFile, lpFileSizeHigh);
}
/***********************************************
GetSaveFileNameW()函数			A获取关闭文件会话框
************************************************/
static BOOL WINAPI New_GetSaveFileNameW(
	_Inout_ LPOPENFILENAME lpofn)
{
	//DP0("@@@@@@@@@@HOOK:  GetSaveFileNameW FUNCTION!!!  ");
	BOOL Ret = getSaveFileNameW(lpofn);
	return Ret;
}
static BOOL WINAPI NewCloseHandle(HANDLE hObject)
{
	//DP1("HOOK:  THE CLOSEHANDLE FUNCTION !!!!  handle = %d", hObject);
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
	//DP0("@@@@@@@@@@HOOK:  BitBlt FUNCTION!!!  ");
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
	//DP3("HOOK测试2New_StretchBlt： hdcDest = %d,hdcSrc = %d,dwRop = %d", hdcDest, hdcSrc, dwRop);
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
	DP0("???????????HOOK: THE NewCreateProcessW FUNCTION !!!!");
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
	DP1("???????????HOOK: THE NewCreateProcessInternal FUNCTION !!!! name = %s", lpApplicationName);
	HANDLE result;
	result = createProcessInternalW(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
							dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);
	InjectDll(lpProcessInformation->dwProcessId, _T("E:\\外发工具\\HookProject_test\\fileHook\\OutGoingFileTool\\Debug\\copyDllHook.dll"));

	return result;
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
	////DP1("end the funcName=%s ", funcName);
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
void hexdump(const unsigned char *buf, const int num)
{
	char *temp = new char[num * 3 + 20];
	char *temp1 = new char[num * 3 * 2 + 20];
	memset(temp1, 0, num * 3 * 2 + 20);
	//DP0("HOOK START:\n");
	int i;
	for (i = 0; i < num; i++)
	{
		//DP2("HOOK %d:%02x ", i, buf[i]);
		sprintf_s(temp + i * 3, 4, "%02X ", buf[i]);
	}
	for (i = 0; i < num * 3; i++) {
		temp1[i * 2] = temp[i];
	}
	DP1("HOOK DUMP!!! %s", temp1);
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
Description: 停止所有的HOOK
************************************************/
void __stdcall EndHook()
{
	//if (GetParentProcessID(GetCurrentProcessId()) == *(int*)pBuffer)
	//{
		//TODO:进行对文件的结尾信息的分析从而去结束对应的HOOK功能
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












