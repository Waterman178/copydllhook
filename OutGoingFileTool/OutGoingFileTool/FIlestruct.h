#pragma once
#pragma pack(4)
#include <windows.h>

typedef struct _Time {
	char startuserTime[20] ;
	char stopuserTime[20] ;
}Time,*PTime;

typedef struct _RjFileSrtuct {
	char FileHeadName[20] ; //文件头的名字
	Time Outgoingfiletime; //文件外发时间记录
	INT Count;//使用次数
	BOOLEAN forbidensaveas;//是否禁用另存为
	BOOLEAN onlyread;//是否只读
}RjFileSrtuct, *pRjFileSrtuct;




