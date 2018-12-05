
// OutGoingFileToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "FIlestruct.h"
#include "lxListBox.h"
#include <memory>
#define   WM_UPDATE_STATIC  (WM_USER + 0x100)


typedef struct _UPDATASTATIC
{
	char * Dlgthis;
	SHELLEXECUTEINFO* pShExecInfo;
}UPDATASTATIC, *PUPDATASTATIC;

static CString GetWorkDir();
static int InjectDll(DWORD dwProcessId, PTCHAR szDllName);
static CString my_Cstr;
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
	afx_msg LRESULT OnUpdateStatic(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedOpenFile();
	//afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButton2();
	lxListBox fileList;
	//lxListBox m_wndListCtrl;
	afx_msg void OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);

public:
	PVOID LoadDllFunc(LPCTSTR lpFileName, LPCSTR lpProcName);
	int CompressFile(const char* comSavewhere, const char* needCom);//压缩文件
	int UncompreFile(const char* uncomTowhere, const char* needUncom);//解压文件
private:
	void(WINAPI *InstallHook)();
	BOOL(WINAPI *UnstallHook)();
private:
	DWORD dwId = 0L;
	HANDLE hid = 0;
	LPVOID pBuffer;                                    // 共享内存指针
	HANDLE hMap;
public:
	std::shared_ptr<rjFileInfo> encryptInfo;
	PVOID TfileName;
	afx_msg void OnHdnEnddragList1(NMHDR *pNMHDR, LRESULT *pResult);
};
