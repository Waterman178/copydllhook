#ifndef __COPYDLLHOOK_H__
#define __COPYDLLHOOK_H__

/*

* All rights reserved.
*
* 当前版本：1.0
* 作者：xjy
* 完成日期：2017年11月16日
*
* copyDllHook.h
* 摘要：HOOK主要操作
*/


#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "detours.h"
#include <Winspool.h>
#include <windef.h>
#include <atlstr.h>
#include <bcrypt.h>
#include <list>
#include <commdlg.h>

#ifdef x64
#pragma comment(lib,"detours_x64.lib")
#else
#pragma comment(lib,"detours.lib")
#endif



//dll库的宏定义
#define USER32 TEXT("User32.dll")
#define WINSPOOL TEXT("WINSPOOL.DRV")
#define PSAPI TEXT("psapi.dll")
#define KERNEL32 TEXT("Kernel32.dll")
#define GDI32 TEXT("Gdi32.dll")
#define NTDLL TEXT("ntdll.dll")
#define KERNELBASE TEXT("KERNELBASE.dll")
#define COMDLG32 TEXT("Comdlg32.dll")


//原函数指针宏定义
#define OPENCLIPBOARD (BOOL(WINAPI *)(HWND ))
#define GETCLIPBOARDDATA (HANDLE (WINAPI *)(UINT ))
#define OPENPRINTER (BOOL (WINAPI *)( LPTSTR ,LPHANDLE ,LPPRINTER_DEFAULTS ))
#define SHOWWINDOW (BOOL(WINAPI*)(HWND, int))
#define SETWINDOWPOS (BOOL(WINAPI *)(HWND, HWND, int, int, int, int, UINT))
#define SETWINDOWTEXTW (BOOL(WINAPI *)(HWND, LPCTSTR))
#define SETWINDOWTEXTA (BOOL(WINAPI *)(HWND, LPCTSTR))
#define CREATEFILEW (HANDLE(WINAPI *)(LPCTSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE))
#define BITBLT (BOOL(WINAPI *)(HDC, int, int, int, int, HDC, int, int, DWORD))
#define STRECTCHBLT (BOOL(WINAPI *)(HDC, int, int, int, int, HDC, int, int, int, int, DWORD))
#define READFILE (int (WINAPI *)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED))
#define WRITEFILE (BOOL(WINAPI *)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED))
#define MAPVIEWOFFILE (LPVOID(WINAPI *)(HANDLE,DWORD,DWORD,DWORD,SIZE_T))
#define MAPVIEWOFFILEEX (LPVOID(WINAPI *)(HANDLE, DWORD, DWORD, DWORD, SIZE_T, LPVOID))
#define CREATEFILEMAPPING (HANDLE (WINAPI *)(HANDLE ,LPSECURITY_ATTRIBUTES ,DWORD ,DWORD ,DWORD ,LPCTSTR ))
#define CREATEFILEMAPPINGA (HANDLE(WINAPI *)(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCSTR))
#define OPENFILEMAPPINGW (HANDLE (WINAPI *)(DWORD,BOOL ,LPCTSTR))
#define ZWCLOSE (NTSTATUS (WINAPI *)(HANDLE))
#define UNMAPVIEWOFFILE (BOOL (WINAPI *)(LPCVOID ))
#define CLOSEHANDLE (BOOL (WINAPI *)(HANDLE ))
#define NTCLOSE (NTSTATUS (NTAPI *)(HANDLE))
#define SETFILEPOINTER (DWORD (WINAPI *)(HANDLE,LONG,PLONG,DWORD))
#define GETFILEINFBYHANDLE (BOOL (WINAPI *)(HANDLE ,LPBY_HANDLE_FILE_INFORMATION ))
#define GETFILESIZEEX (BOOL (WINAPI *)(HANDLE ,PLARGE_INTEGER ))
#define SETFILEPOINTER_EX  (BOOL (WINAPI *)(HANDLE ,LARGE_INTEGER  ,PLARGE_INTEGER ,DWORD ))
#define GETFILESIZE (DWORD (WINAPI *)(HANDLE ,LPDWORD ))
#define GETSAVEFILENAMEW (BOOL (WINAPI *)(LPOPENFILENAME ))
#define GETFILEATTRIBUTESW (DWORD (WINAPI *)(LPCTSTR ))
#define READFILEEX (BOOL(WINAPI *)(HANDLE , LPVOID , DWORD , LPOVERLAPPED , LPOVERLAPPED_COMPLETION_ROUTINE ))
#define READFILESCATTER (BOOL (WINAPI *)(HANDLE ,FILE_SEGMENT_ELEMENT* ,DWORD ,LPDWORD ,LPOVERLAPPED))
#define GETFILEATTRIBUTESEXW (DWORD(WINAPI *)(LPCTSTR ))
#define CREATEPROCESS  (BOOL(WINAPI *)(LPCTSTR ,LPTSTR ,LPSECURITY_ATTRIBUTES ,LPSECURITY_ATTRIBUTES ,BOOL ,DWORD ,LPVOID ,LPCTSTR ,LPSTARTUPINFO ,LPPROCESS_INFORMATION ))
#define PROCESSINTERNALW (HANDLE (WINAPI *)(HANDLE , LPCTSTR , LPTSTR , LPSECURITY_ATTRIBUTES , LPSECURITY_ATTRIBUTES , BOOL , DWORD , LPVOID , LPCTSTR , LPSTARTUPINFOA , LPPROCESS_INFORMATION , PHANDLE ))


//原函数的函数指针声明



static BOOL (WINAPI *openClipboard)(HWND hWndNewOwner);
static HANDLE (WINAPI *getClipboardData)(UINT uFormat);
static BOOL (WINAPI *openPrinter)( LPTSTR pPrinterName,LPHANDLE phPrinter,LPPRINTER_DEFAULTS pDefault);
static BOOL(WINAPI* showWindow)(HWND hWnd, int uType);
static BOOL(WINAPI *setWindowPos)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx,int cy, UINT uFlags);
static BOOL (WINAPI *setWindowTextW)(HWND hwnd, LPCTSTR lpString);
static BOOL(WINAPI *setWindowTextA)(HWND hwnd, LPCTSTR lpString);
static HANDLE (WINAPI *createFileW)( LPCTSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD  dwFlagsAndAttributes,HANDLE hTemplateFile);
static BOOL (WINAPI *bitBlt)(HDC hdcDest,int nXDest,int nYDest,int nWidth,int nHeight, HDC hdcSrc,int nXSrc,int nYSrc,DWORD dwRop);
static BOOL (WINAPI *stretchBlt)(HDC hdcDest, int nXOriginDest,int nYOriginDest, int nWidthDest,int nHeightDest, HDC hdcSrc,int nXOriginSrc, int nYOriginSrc, int nWidthSrc,int nHeightSrc, DWORD dwRop);
static int (WINAPI *readfile)(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped);
static BOOL (WINAPI *writeFile)(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped);
static LPVOID (WINAPI *mapViewOfFile)(HANDLE hFileMappingObject,DWORD dwDesiredAccess,DWORD dwFileOffsetHigh,DWORD dwFileOffsetLow,SIZE_T dwNumberOfBytesToMap);
static LPVOID (WINAPI *mapViewOfFileEx)(HANDLE hFileMappingObject,DWORD dwDesiredAccess,DWORD dwFileOffsetHigh,DWORD dwFileOffsetLow,SIZE_T dwNumberOfBytesToMap,LPVOID lpBaseAddress);
static HANDLE (WINAPI *createFileMapping)(HANDLE hFile,LPSECURITY_ATTRIBUTES lpAttributes,DWORD flProtect,DWORD dwMaximumSizeHigh,DWORD dwMaximumSizeLow,LPCTSTR lpName);
static HANDLE (WINAPI *openFileMappingW)(DWORD   dwDesiredAccess,BOOL bInheritHandle,LPCTSTR lpName);
static NTSTATUS (WINAPI *zwClose)(HANDLE Handle);
static BOOL (WINAPI *unmapViewOfFile)(LPCVOID lpBaseAddress);
static BOOL (WINAPI *closeHandle)(HANDLE hObject);
static NTSTATUS (NTAPI *ntClose)(HANDLE);
static DWORD (WINAPI *setFilePointer)(HANDLE hFile,LONG   lDistanceToMove,PLONG  lpDistanceToMoveHigh,DWORD  dwMoveMethod);
static BOOL (WINAPI *getFileInformationByHandle)(HANDLE hFile,LPBY_HANDLE_FILE_INFORMATION lpFileInformation);
static BOOL (WINAPI *getFileSizeEx)(HANDLE  hFile,PLARGE_INTEGER lpFileSize);
static BOOL (WINAPI *setFilePointerEx)(HANDLE hFile,LARGE_INTEGER  liDistanceToMove,PLARGE_INTEGER lpNewFilePointer,DWORD dwMoveMethod);
static DWORD (WINAPI *getFileSize)(HANDLE hFile,LPDWORD lpFileSizeHigh);
static BOOL (WINAPI *getSaveFileNameW)(LPOPENFILENAME lpofn);
static DWORD (WINAPI *getFileAttributesW)(LPCTSTR lpFileName);
static BOOL (WINAPI *readFileEx)(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPOVERLAPPED lpOverlapped,LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
static BOOL (WINAPI *readFileScatter)(HANDLE hFile,FILE_SEGMENT_ELEMENT aSegmentArray[],DWORD nNumberOfBytesToRead,LPDWORD lpReserved,LPOVERLAPPED lpOverlapped);
static DWORD (WINAPI *getFileAttributesExW)( LPCTSTR lpFileName);
static BOOL (WINAPI *createProcessW)(LPCTSTR lpApplicationName,LPTSTR lpCommandLine,LPSECURITY_ATTRIBUTES lpProcessAttributes,LPSECURITY_ATTRIBUTES lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,LPVOID lpEnvironment,LPCTSTR lpCurrentDirectory,LPSTARTUPINFO lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation);
static HANDLE (WINAPI *createProcessInternalW)(HANDLE hToken, LPCTSTR lpApplicationName, LPTSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hNewToken);



static
HANDLE(WINAPI*createfilemappingA)(
	__in     HANDLE hFile,
	__in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
	__in     DWORD flProtect,
	__in     DWORD dwMaximumSizeHigh,
	__in      DWORD dwMaximumSizeLow,
	__in_opt LPCSTR lpName
);
//被HOOK的函数新功能，函数集合
static BOOL WINAPI NewOpenClipboard(_In_opt_ HWND hWndNewOwner);//OpenClipboard打开剪切板
static HANDLE WINAPI NewGetClipboardData(_In_ UINT uFormat);	//GetClipboardData()函数获取剪切板的内容
static BOOL WINAPI NewOpenPrinter(__in LPTSTR pPrinterName,
	__out LPHANDLE phPrinter,
	__in LPPRINTER_DEFAULTS pDefault);							//OpenPrinter()函数 打印功能
static BOOL WINAPI NewShowWindow(HWND hWnd,int nCmdShow);		//showWindow()函数 另存为功能
static BOOL WINAPI NewSetWindowPos(_In_     HWND hWnd,_In_opt_ HWND hWndInsertAfter,
	_In_     int  X,_In_     int  Y,
	_In_     int  cx,_In_     int  cy,_In_     UINT uFlags
	);															//SetWindowPos()函数 改变一个子窗口，弹出式窗口或顶层窗口的尺寸，位置和Z序功能
static BOOL WINAPI NewSetWindowTextW(HWND hwnd, LPCTSTR lpString);		//设置对话框标题或者对话框控件文本的内容
static BOOL WINAPI NewSetWindowTextA(HWND hwnd, LPCTSTR lpString);		//设置对话框标题或者对话框控件文本的内容
static HANDLE WINAPI NewCreateFileW(
	_In_     LPCTSTR               lpFileName,
	_In_     DWORD                 dwDesiredAccess,
	_In_     DWORD                 dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_     DWORD                 dwCreationDisposition,
	_In_     DWORD                 dwFlagsAndAttributes,
	_In_opt_ HANDLE                hTemplateFile);//函数创建或打开下列对象
static BOOL WINAPI New_BitBlt(_In_ HDC   hdcDest,
	_In_ int   nXDest,_In_ int   nYDest,	_In_ int   nWidth,
	_In_ int   nHeight,_In_ HDC   hdcSrc,_In_ int   nXSrc,
	_In_ int   nYSrc,_In_ DWORD dwRop);			//截图功能
static BOOL WINAPI New_StretchBlt(HDC hdcDest, int nXOriginDest,
	int nYOriginDest, int nWidthDest, 
	int nHeightDest, HDC hdcSrc, 
	int nXOriginSrc, int nYOriginSrc, int nWidthSrc, 
	int nHeightSrc, DWORD dwRop);
static int WINAPI NewReadfile(
	_In_        HANDLE       hFile,
	_Out_       LPVOID       lpBuffer,
	_In_        DWORD        nNumberOfBytesToRead,
	_Out_opt_   LPDWORD      lpNumberOfBytesRead,
	_Inout_opt_ LPOVERLAPPED lpOverlapped);
static BOOL WINAPI New_WriteFile(
	HANDLE hFile,                    // handle to file
	LPCVOID lpBuffer,                // data buffer
	DWORD nNumberOfBytesToWrite,     // number of bytes to write
	LPDWORD lpNumberOfBytesWritten,  // number of bytes written
	LPOVERLAPPED lpOverlapped        // overlapped buffer
	);
static LPVOID WINAPI NewMapViewOfFile(
	__in HANDLE hFileMappingObject,
	__in DWORD dwDesiredAccess,
	__in DWORD dwFileOffsetHigh,
	__in DWORD dwFileOffsetLow,
	__in SIZE_T dwNumberOfBytesToMap
	);
static LPVOID WINAPI NewMapViewOfFileEx(
	__in HANDLE hFileMappingObject,
	__in DWORD dwDesiredAccess,
	__in DWORD dwFileOffsetHigh,
	__in DWORD dwFileOffsetLow,
	__in SIZE_T dwNumberOfBytesToMap,
	__in LPVOID lpBaseAddress
	);
static HANDLE WINAPI NewCreateFileMapping(
	_In_ HANDLE hFile,
	_In_opt_ LPSECURITY_ATTRIBUTES lpAttributes,
	_In_ DWORD flProtect,
	_In_ DWORD dwMaximumSizeHigh,
	_In_ DWORD dwMaximumSizeLow,
	_In_opt_ LPCTSTR lpName);
static HANDLE WINAPI NewOpenFileMappingW(
	_In_ DWORD   dwDesiredAccess,
	_In_ BOOL    bInheritHandle,
	_In_ LPCTSTR lpName);
static BOOL WINAPI NewCloseHandle(
	HANDLE hObject   // handle to object  要关闭的文件句柄
	);
static NTSTATUS WINAPI New_ZwClose(_In_ HANDLE Handle);
static NTSTATUS NTAPI NewNtClose(IN HANDLE Handle);
static BOOL WINAPI NewUnmapViewOfFile(_In_ LPCVOID lpBaseAddress);
static DWORD WINAPI NewSetFilePointer(
	_In_        HANDLE hFile,
	_In_        LONG   lDistanceToMove,
	_Inout_opt_ PLONG  lpDistanceToMoveHigh,
	_In_        DWORD  dwMoveMethod);
static BOOL WINAPI NewGetFileInformationByHandle(
	_In_  HANDLE                       hFile,
	_Out_ LPBY_HANDLE_FILE_INFORMATION lpFileInformation);
static BOOL WINAPI NewGetFileSizeEx(
	_In_  HANDLE         hFile,
	_Out_ PLARGE_INTEGER lpFileSize);
static BOOL WINAPI NewSetFilePointerEx(
	_In_      HANDLE         hFile,
	_In_      LARGE_INTEGER  liDistanceToMove,
	_Out_opt_ PLARGE_INTEGER lpNewFilePointer,
	_In_      DWORD          dwMoveMethod);
static DWORD WINAPI NewGetFileSize(
	HANDLE hFile,
	LPDWORD lpFileSizeHigh);
static BOOL WINAPI New_GetSaveFileNameW(
	_Inout_ LPOPENFILENAME lpofn);
static DWORD WINAPI NewGetFileAttributesW(
	_In_ LPCTSTR lpFileName	);
static BOOL WINAPI NewReadFileEx(
	_In_      HANDLE                          hFile,
	_Out_opt_ LPVOID                          lpBuffer,
	_In_      DWORD                           nNumberOfBytesToRead,
	_Inout_   LPOVERLAPPED                    lpOverlapped,
	_In_      LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	);
static BOOL WINAPI NewReadFileScatter(
	_In_       HANDLE               hFile,
	_In_       FILE_SEGMENT_ELEMENT aSegmentArray[],
	_In_       DWORD                nNumberOfBytesToRead,
	_Reserved_ LPDWORD              lpReserved,
	_Inout_    LPOVERLAPPED         lpOverlapped
	);
static DWORD WINAPI NewGetFileAttributesExW(
	__in LPCTSTR lpFileName
	);


//用createProcess实现被动注入

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
	);
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
);
static HANDLE WINAPI NewCreateProcessInternal(HANDLE hToken, LPCTSTR lpApplicationName, LPTSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hNewToken);

//HOOK功能集函数
PVOID FindProcAddress(_In_ LPCTSTR lpFileName, LPCSTR lpProcName);
PVOID StartOneHook(LPCTSTR dllName, LPCSTR funcName, PVOID newFunc);
void EndOneHook(LPCTSTR dllName, PVOID oldFunc, PVOID newFunc);
int rc4(char *pSecret, int SecretLen, char *pMessage, int MessageLen, char *pOut);//rc4  加密解密算法
void hexdump(const unsigned char *buf, const int num);//16进制转储


//DLL，导出函数
void __stdcall StartHook();
void __stdcall EndHook();



void OutputDebugStringEx(const char *strOutputString, ...);


#endif//__COPYDLLHOOK_H__