#pragma once
#pragma pack(4)
#include <windows.h>

#define DLLTMP "C:\\Users\\Administrator\\AppData\\Local\\Temp\\copyDllHook.dll"
#define RARTMP "C:\\Users\\Administrator\\AppData\\Local\\Temp\\Rar.exe "
#define UNRARTMP "C:\\Users\\Administrator\\AppData\\Local\\Temp\\UnRAR.exe"
#define TMPDIR "C:\\Users\\Administrator\\AppData\\Local\\Temp\\RJUNCOMOUTGONINGFILETOOL__"
#define E_TMPDIR "E:\\XUJYALDSKFJLSKDFJKLSDFJALSDKFJLKSD"

#define FileName "RjiSafe9575"

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
	ULONG length; //����buffter�ĳ��ȣ�
}RjFileSrtuct, *pRjFileSrtuct;

//Ŀ������ļ���ӵ�е���Ϣ
typedef struct  _rjFileInfo {
	RjFileSrtuct encryptHead;
	char *  EncryptBuffer; //���ܺ��buffer  //����Ҳ�����ֽڶ�����
}rjFileInfo,*prjFileInfo;



