
// OutGoingFileToolDlg.h : ͷ�ļ�
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
// COutGoingFileToolDlg �Ի���
class COutGoingFileToolDlg : public CDialogEx
{
// ����
public:
	COutGoingFileToolDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~COutGoingFileToolDlg();//������������
// �Ի�������
	enum { IDD = IDD_OUTGOINGFILETOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	// ���ɵ���Ϣӳ�亯��
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
	int CompressFile(const char* comSavewhere, const char* needCom);//ѹ���ļ�
	int UncompreFile(const char* uncomTowhere, const char* needUncom);//��ѹ�ļ�
private:
	void(WINAPI *InstallHook)();
	BOOL(WINAPI *UnstallHook)();
private:
	DWORD dwId = 0L;
	HANDLE hid = 0;
	LPVOID pBuffer;                                    // �����ڴ�ָ��
	HANDLE hMap;
public:
	std::shared_ptr<rjFileInfo> encryptInfo;
	PVOID TfileName;
	afx_msg void OnHdnEnddragList1(NMHDR *pNMHDR, LRESULT *pResult);
};
