
// OutGoingFileToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "FIlestruct.h"
#include <memory>
#define DLLTMP "C:\\Users\\Administrator\\AppData\\Local\\Temp\\copyDllHook.dll"
#define RARTMP "C:\\Users\\Administrator\\AppData\\Local\\Temp\\Rar.exe "
#define UNRARTMP "C:\\Users\\Administrator\\AppData\\Local\\Temp\\UnRAR.exe"
#define TMPDIR "C:\\Users\\Administrator\\AppData\\Local\\Temp\\RJUNCOMOUTGONINGFILETOOL__"
#define E_TMPDIR "E:\\XUJYALDSKFJLSKDFJKLSDFJALSDKFJLKSD"

#define FileName "RjiSafe9575"
// COutGoingFileToolDlg 对话框
class COutGoingFileToolDlg : public CDialogEx
{
// 构造
public:
	COutGoingFileToolDlg(CWnd* pParent = NULL);	// 标准构造函数
	~COutGoingFileToolDlg();//界面析构函数
// 对话框数据
	enum { IDD = IDD_OUTGOINGFILETOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenFile();
	afx_msg void OnBnClickedButton2();
	CListCtrl fileList;
	afx_msg void OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);

public:

	int InjectDll(DWORD dwProcessId, PTCHAR szDllName);//开始dll远程注入操作
	PVOID LoadDllFunc(LPCTSTR lpFileName, LPCSTR lpProcName);
	int CompressFile(const char* comSavewhere, const char* needCom);//压缩文件
	int UncompreFile(const char* uncomTowhere, const char* needUncom);//解压文件
	char* CString2char(CString str);
private:
	void(WINAPI *InstallHook)();
	BOOL(WINAPI *UnstallHook)();

private:
	DWORD dwId = 0L;
	HANDLE hid = 0;
	LPVOID pBuffer;                                    // 共享内存指针
	HANDLE hMap;
public:
	
	std::shared_ptr<RjFileSrtuct> encryptHead;
	PVOID TfileName;
};
