#pragma once
#pragma pack(1)
#include <windows.h>

typedef struct _Time {
	char startuserTime[20] ;
	char stopuserTime[20] ;
}Time,*PTime;

//����Ǽ��ܺ���ļ�д���ļ�ͷ���information
typedef struct _RjFileSrtuct {
	char FileHeadName[20] ; //�ļ�ͷ������
	char FileSrcName [60];  //ԭ���ļ�������
	Time Outgoingfiletime; //�ļ��ⷢʱ���¼
	INT Count;//ʹ�ô���
	BOOLEAN forbidensaveas;//�Ƿ�������Ϊ
	BOOLEAN onlyread;//�Ƿ�ֻ��
}RjFileSrtuct, *pRjFileSrtuct;

//Ŀ������ļ���ӵ�е���Ϣ
typedef struct  _rjFileInfo {
	RjFileSrtuct encryptHead;
	PVOID EncryptBuffer; //���ܺ��buffer
}rjFileInfo,*prjFileInfo;



