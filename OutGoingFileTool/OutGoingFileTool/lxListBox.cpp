// lxListBox.cpp: 实现文件
//

#include "stdafx.h"
#include "OutGoingFileTool.h"
#include "lxListBox.h"
#include "afxdialogex.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

// lxListBox 对话框

IMPLEMENT_DYNAMIC(lxListBox, CListCtrl)

lxListBox::lxListBox(CWnd* pParent /*=nullptr*/)
	: CListCtrl()
{

}

lxListBox::~lxListBox()
{
}

void lxListBox::DoDataExchange(CDataExchange* pDX)
{
	CListCtrl::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(lxListBox, CListCtrl)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


BOOL  lxListBox::CheackTime(const char* str) {
	int Y, M, D, h, m, s;
	CTime today;
	CTime localtoday;
	sscanf(str, "%d-%d-%d %d:%d:%d", &Y, &M, &D, &h, &m, &s);
	today = CTime(Y, M, D, h, m, s);
	localtoday = CTime::GetCurrentTime();
	CTimeSpan span = localtoday - today; //计算当前系统时间与时间t1的间隔
	//span大于68年时候会溢出
	int iMin = span.GetTotalMinutes();//获取总共有多少分钟
	if (iMin > 0)
	{
		::MessageBox(NULL, _T("打开文档失败，外发到期"), _T("提示"), MB_YESNO | MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}
// lxListBox 消息处理程序
VOID lxListBox::OnDropFiles(HDROP hDropInfo) {
	UINT  nNumOfFiles = DragQueryFileA(hDropInfo, 0xFFFFFFFF, NULL, 0); //文件的个数
	char  decryptbuffer[260] = { 0 };
	char  cFilePathName[250] = { 0 };
	char  pBuffer[250] = { 0 };
	char  Filepullpath[250] = { 0 };
	char  strsize[9] = { 0 };
	CTime tmScan;
	memcpy(Filepullpath, GetWorkDir().GetBuffer(), 250);
	for (UINT nIndex = 0; nIndex < nNumOfFiles; ++nIndex)
	{
		FileInfo = std::shared_ptr<rjFileInfo>(new rjFileInfo());
		DragQueryFileA(hDropInfo, nIndex, cFilePathName, _MAX_PATH);  //得到文件名
		LPARAM lParam = (LPARAM)cFilePathName;
		SetFileAttributesA(cFilePathName, FILE_ATTRIBUTE_NORMAL);
		char buffer[20] = { 0 };
		USES_CONVERSION;
		try
		{
			FILE * TEMP = fopen(cFilePathName, "rb+");//二进制的形式读取
			if (!TEMP)
				printf("open fail errno = %d reason = %s \n", errno, strerror(errno));
			for (int i = 0; i <= 20; i++)
			{
				auto status = fread(buffer + i, 1, 1, TEMP);
			}
			fseek(TEMP, 0, SEEK_SET);
			if (strstr(buffer, FileName) != NULL)
			{
				auto length = sizeof(RjFileSrtuct);
				for (ULONG i = 0; i < length; i++)
				{
					auto status = fread(FileInfo->encryptHead.FileHeadName + i, 1, 1, TEMP);
				}
				int encryptlength = (int)FileInfo->encryptHead.length;
				if (!FileInfo->encryptHead.Count)
				{
					::MessageBox(NULL, _T("打开文档失败，使用次数为0"), _T("提示"), MB_YESNO | MB_ICONEXCLAMATION);
					fclose(TEMP);
					return;
				}
				else {
					FileInfo->encryptHead.Count--;
					fseek(TEMP, 0, SEEK_SET);
					for (ULONG i = 0; i < length; i++)
					{
						auto status = fwrite(FileInfo->encryptHead.FileHeadName + i, 1, 1, TEMP);
						fflush(TEMP);
					}
				}
				if (!CheackTime(FileInfo->encryptHead.Outgoingfiletime.stopuserTime))
				{
					fclose(TEMP);
					return;
				}
				strcat(Filepullpath, FileInfo->encryptHead.FileSrcName);
				fclose(TEMP);
				if (CopyFile(cFilePathName, Filepullpath, TRUE))
					SetFileAttributes(Filepullpath, FILE_ATTRIBUTE_READONLY);
			}
			else
			{
				fclose(TEMP);
			}
		}
		catch (CMemoryException* e)
		{
			;
		}
		catch (CFileException* e)
		{
			;
		}
		catch (CException* e)
		{
			;
		}
		itoa(FileInfo->encryptHead.length, strsize, 10);
		tmScan = CTime::GetCurrentTime();
		CString str =   tmScan.Format("%Y-%m-%d %H:%M:%S");
		InsertItem(0,(LPCTSTR)FileInfo->encryptHead.FileSrcName);// 在第一列插入文件名
		SetItemText(0, 1, (LPCTSTR)str.GetBuffer());
		SetItemText(0, 2, (LPCTSTR)strsize);
	}
	DragFinish(hDropInfo);
}

