#pragma once
#pragma pack(4)
#include <windows.h>

typedef struct _Time {
	char startuserTime[20] ;
	char stopuserTime[20] ;
}Time,*PTime;

typedef struct _RjFileSrtuct {
	char FileHeadName[20] ; //�ļ�ͷ������
	Time Outgoingfiletime; //�ļ��ⷢʱ���¼
	INT Count;//ʹ�ô���
	BOOLEAN forbidensaveas;//�Ƿ�������Ϊ
	BOOLEAN onlyread;//�Ƿ�ֻ��
}RjFileSrtuct, *pRjFileSrtuct;




