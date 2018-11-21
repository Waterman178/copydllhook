#ifndef _DEBUGPRINTF_H_ 
#define _DEBUGPRINTF_H_ 
#pragma once
#include<Windows.h> 
#include <tchar.h> 

//用于输出信息到编译器输出窗口的宏定义 
//使用win API，DEBUG版本会执行，RELEASE版本则不会 
//还可以使用DebugView，WinDbg等工具查看输出 

//#ifdef _DEBUG 
//
//#define OutputDebugStringEx(fmt) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt));OutputDebugString(sOut);} 
//#define OutputDebugStringEx(fmt,var) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var);OutputDebugString(sOut);} 
//#define OutputDebugStringEx(fmt,var1,var2) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var1,var2);OutputDebugString(sOut);} 
//#define OutputDebugStringEx(fmt,var1,var2,var3) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var1,var2,var3);OutputDebugString(sOut);} 
//
//#endif 
//
//#ifndef _DEBUG 
//
//#define OutputDebugStringEx(fmt) ; 
//#define OutputDebugStringEx(fmt, var) ; 
//#define OutputDebugStringEx(fmt,var1,var2) ; 
//#define OutputDebugStringEx(fmt,var1,var2,var3) ; 
//
//#endif 
//
#endif 


