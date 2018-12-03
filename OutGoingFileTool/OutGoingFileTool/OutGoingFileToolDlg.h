
// OutGoingFileToolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "FIlestruct.h"
#include "lxListBox.h"
#include <memory>

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

	int InjectDll(DWORD dwProcessId, PTCHAR szDllName);//��ʼdllԶ��ע�����
	PVOID LoadDllFunc(LPCTSTR lpFileName, LPCSTR lpProcName);
	int CompressFile(const char* comSavewhere, const char* needCom);//ѹ���ļ�
	int UncompreFile(const char* uncomTowhere, const char* needUncom);//��ѹ�ļ�
	CString GetWorkDir();
	//char* CString2char(CString str);
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
