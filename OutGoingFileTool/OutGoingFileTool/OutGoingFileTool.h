
// OutGoingFileTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// COutGoingFileToolApp: 
// �йش����ʵ�֣������ OutGoingFileTool.cpp
//

class COutGoingFileToolApp : public CWinApp
{
public:
	COutGoingFileToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern COutGoingFileToolApp theApp;