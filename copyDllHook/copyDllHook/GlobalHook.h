#ifndef __GLOBALHOOK_H__
#define __GLOBALHOOK_H__
#pragma once
#include "stdafx.h"


//键盘钩子的回调函数
LRESULT CALLBACK FunKey(
	int code, // hook code
	WPARAM wParam, // virtual-key code
	LPARAM lParam // keystroke-message information
	);

//信息钩子的回调函数
LRESULT CALLBACK MyMessageProc(int code, WPARAM wParam, LPARAM lParam);



void __stdcall StartHookKeyBord();
BOOL __stdcall EndHookKeyBord();

void __stdcall StartHookMsg();
BOOL __stdcall EndHookMsg();

int InjectDll(DWORD dwProcessId, PTCHAR szDllName);










#endif//__GLOBALHOOK_H__