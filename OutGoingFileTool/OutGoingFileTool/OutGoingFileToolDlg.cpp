
// OutGoingFileToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "OutGoingFileTool.h"
#include "OutGoingFileToolDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include <stdio.h>
#include <TlHelp32.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// COutGoingFileToolDlg �Ի���



COutGoingFileToolDlg::COutGoingFileToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COutGoingFileToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
COutGoingFileToolDlg::~COutGoingFileToolDlg()
{
	//ж�ؼ��̵�PRINT SCREEN �����Ŀ���
	UnstallHook = (BOOL (WINAPI*)())LoadDllFunc(_T("copyDllHook.dll"), "EndHookKeyBord");
	UnstallHook();

	//ж��ȫ����Ϣ����
	UnstallHook = (BOOL(WINAPI*)())LoadDllFunc(_T("copyDllHook.dll"), "EndHookMsg");
	UnstallHook();
	//�ر��Ѵ򿪵��ļ�
	//::SendMessage(, WM_SYSCOMMAND, SC_CLOSE, 0);
	TerminateProcess(hid,0);
	//ɾ����ʱ�ļ��н�ѹ�������ļ�
	if (SetCurrentDirectory(_T(TMPDIR)) == TRUE)
	{
		CFileFind  finder;
		BOOL bWorking = finder.FindFile(_T("*.*"));
		int line = 0;
		CTime tmp;
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (line > 1)
			{
				HWND hwnd = ::FindWindow(NULL, finder.GetFileName());
				DeleteFile(finder.GetFilePath());		
			}
			line++;
		}
	}
	::UnmapViewOfFile(pBuffer);
	::CloseHandle(hMap);
}

void COutGoingFileToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, fileList);
}

BEGIN_MESSAGE_MAP(COutGoingFileToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &COutGoingFileToolDlg::OnBnClickedOpenFile)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &COutGoingFileToolDlg::OnHdnItemclickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &COutGoingFileToolDlg::OnNMDblclkList1)
END_MESSAGE_MAP()


// COutGoingFileToolDlg ��Ϣ��������

BOOL COutGoingFileToolDlg::OnInitDialog()
{
	int Ret = ::MessageBox(NULL, _T("��ر����ڴ򿪵��ĵ������ڲ鿴�����ļ���ʱ�رմ˳���������������"), _T("��ʾ"), MB_YESNO | MB_ICONEXCLAMATION);
	if (Ret == 7)
		exit(0);
	CDialogEx::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ����Ӷ���ĳ�ʼ������
	CreateDirectory(_T(E_TMPDIR), NULL);


	fileList.InsertColumn(0, _T("�ļ���"), LVCFMT_CENTER, 160, 0);
	fileList.InsertColumn(1, _T("�޸�ʱ��"), LVCFMT_CENTER, 160, 1);
	fileList.InsertColumn(2, _T("�ļ���С/KB"), LVCFMT_CENTER, 160, 2);
	fileList.InsertColumn(3, _T(""), LVCFMT_CENTER, 0,3);

	//������ʱ�ļ���
	int ret = CreateDirectory(_T(E_TMPDIR), 0);
	if (ret == 0) {
		printf("CreateDirectory fail(%d)\n", GetLastError());
	}
	else {
		printf("CreateDirectory -> %d\n", ret);
	}

	hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(int),
		_T("processMem_FUCK"));
	// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������
	pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	*(int*)pBuffer = GetCurrentProcessId();
	
	////���̵�PRINT SCREEN �����Ŀ���
	//InstallHook = (void (WINAPI*)())LoadDllFunc(_T("copyDllHook.dll"), "StartHookKeyBord");
	//InstallHook();
	//
	////ȫ����Ϣ����
	//InstallHook = (void (WINAPI*)())LoadDllFunc(_T("copyDllHook.dll"), "StartHookMsg");
	//InstallHook();
	LoadLibraryA("copyDllHook.dll");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void COutGoingFileToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void COutGoingFileToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR COutGoingFileToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void COutGoingFileToolDlg::OnBnClickedOpenFile()
{

	////�����ļ�

	/*FILE * TEMP = fopen("E:\\XUJYALDSKFJLSKDFJKLSDFJALSDKFJLKSD\\test001.rar", "rb+");
	FILE * TEMP1 = fopen("E:\\XUJYALDSKFJLSKDFJKLSDFJALSDKFJLKSD\\rar001.rar", "ab+");

	char buf[20] = { 0 };
	while (fread(buf, 1, 1, TEMP)) {
		buf[0] ^= 'a';
		fwrite(buf, 1, 1, TEMP1);
	}
	char scretInfo[] = { "DSKFJLSKDF" };
	int iflag = 0;
	while (iflag < 10)
	{
		fwrite(&scretInfo[iflag], 1, 1, TEMP1);
		iflag++;
	}
	fflush(TEMP1);
	fclose(TEMP);
	fclose(TEMP1);
	return;*/


	char ucomDirPath[2048] = { 0 };

	//���ù�����
	TCHAR szFilter[] = _T("rar�ļ�(*.rar)|*.rar|ȫ���ļ�(*.*)|*.*");
	CFileDialog dlg(TRUE, _T("doc"), _T("my"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		//�������һ���ļ������ƣ���ѹѡ���ļ������Ŀ¼��
		//memcpy(ucomDirPath,GetRandLetter(),15);
		//sprintf(ucomDirPath, "%s",GetRandLetter());
		int ret = UncompreFile(E_TMPDIR, CString2char(dlg.GetPathName()));
		Sleep(100);														//TODO:���ļ���ʱ��о����ǻ�������
		//if (ret > 31)
		//{
			if (SetCurrentDirectory(_T(E_TMPDIR)) == TRUE)
			{
				OutputDebugString(_T("find the file>>>>>>>>>>>>"));
				CFileFind  finder;
				CString fileInfo;
				BOOL bWorking = finder.FindFile(_T("*.*"));
				int line = 0;
				CTime tmp;
				while (bWorking)
				{
					bWorking = finder.FindNextFile();
					if (line > 1)
					{
						OutputDebugString(_T("find the file"));
						fileList.InsertItem(line - 2, finder.GetFileName());
						finder.GetLastWriteTime(tmp);
						fileInfo.Format(_T("%d/%d/%d"), tmp.GetYear(), tmp.GetMonth(), tmp.GetDay());
						fileList.SetItemText(line - 2, 1, fileInfo);
						fileInfo.Format(_T("%ld"), finder.GetLength());
						fileList.SetItemText(line - 2, 2, fileInfo);
						fileList.SetItemText(line - 2, 3, finder.GetFilePath());
					}
					line++;
				}
			}
			else
			{
				return;
			}
		//}
		

	}
	
}


void COutGoingFileToolDlg::OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	*pResult = 0;

	CString strLangName;//ѡ�����Ե������ַ���
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;

	if (-1 != pNMListView->iItem)
	{
		//��ȡ��ѡ���б��ĵ�һ��������ı�
		strLangName = fileList.GetItemText(pNMListView->iItem, 3);
		//��ѡ���������ʾ��༭����
		//SetDlgItemText(IDC_LANG_SEL_EDIT,strLangName);
		ShellExecute(NULL, _T("open"), strLangName, NULL, NULL, SW_SHOWNORMAL);
	}
}





void COutGoingFileToolDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	*pResult = 0;

	CString strLangName;//ѡ�����Ե������ַ���
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;


	if (-1 != pNMListView->iItem)
	{
		//��ȡ��ѡ���б��ĵ�һ��������ı�
		strLangName = fileList.GetItemText(pNMListView->iItem, 3);
		//TODO:��ѹ���ļ�

		//��ѡ���������ʾ��༭����
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = strLangName;    //Ҫ���е��ļ�
		ShExecInfo.lpParameters = _T("");
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOW;
		ShExecInfo.hInstApp = NULL;
		DWORD dwId = 0L;
		if (ShellExecuteEx(&ShExecInfo))
		{
			hid = ShExecInfo.hProcess;
			dwId = ::GetProcessId(ShExecInfo.hProcess);//��ȡ�򿪵���һ������Ľ���ID
			
			//InjectDll(dwId, _T("E:\\�ⷢ����\\HookProject_test\\fileHook\\OutGoingFileTool\\Debug\\copyDllHook.dll"));
		}

	}
}
int COutGoingFileToolDlg::InjectDll(DWORD dwProcessId, PTCHAR szDllName)
{
	if (szDllName[0] == NULL)
		return -1;
	//���Ȩ����ز���  
	//EnablePrivilege(TRUE);
	//1. �򿪽���  
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,   //�򿪽���Ȩ��  
		FALSE,                                              //�Ƿ�ɼ̳�   
		dwProcessId);                                       //����ID  

	if (hProcess == INVALID_HANDLE_VALUE)
		return -1;

	//2. ��Զ�̽���������ռ�  
	LPVOID pszDllName = ::VirtualAllocEx(hProcess, //Զ�̽��̾��  
		NULL,                                  //���鿪ʼ��ַ  
		4096,                                  //����ռ��С  
		MEM_COMMIT,                            //�ռ��ʼ��ȫ0  
		PAGE_EXECUTE_READWRITE);               //�ռ�Ȩ��  

	if (NULL == pszDllName)
	{
		return -1;
	}

	//3. ��Զ�̽�����д������  
	BOOL bRet = ::WriteProcessMemory(hProcess, pszDllName,
		szDllName, MAX_PATH, NULL);

	if (NULL == bRet)
	{
		return -1;
	}

	//4. ��Զ�̽����д���Զ���߳� 
	HANDLE m_hInjecthread = NULL;
	m_hInjecthread = ::CreateRemoteThread(hProcess,      //Զ�̽��̾��  
		NULL,                                            //��ȫ����  
		0,                                               //ջ��С  
		(LPTHREAD_START_ROUTINE)LoadLibrary,             //���̴�������      
		pszDllName,                                      //�������  
		NULL,                                            //Ĭ�ϴ������״̬  
		NULL);                                           //�߳�ID  

	if (NULL == m_hInjecthread)
	{
		DWORD dwErr = GetLastError();
		return -1;
	}

	//5. �ȴ��߳̽�������  
	DWORD dw = WaitForSingleObject(m_hInjecthread, -1);
	//6. ��ȡ�߳��˳���,��LoadLibrary�ķ���ֵ����dll���׵�ַ  
	DWORD dwExitCode;
	HMODULE m_hMod;
	GetExitCodeThread(m_hInjecthread, &dwExitCode);
	m_hMod = (HMODULE)dwExitCode;

	//7. �ͷſռ�  
	BOOL bReturn = VirtualFreeEx(hProcess, pszDllName,
		4096, MEM_DECOMMIT);

	if (NULL == bReturn)
	{
		return -1;
	}

	CloseHandle(hProcess);
	hProcess = NULL;
	//�ָ�Ȩ����ز���  
	//EnablePrivilege(FALSE);

	return 0;
}
PVOID COutGoingFileToolDlg::LoadDllFunc(LPCTSTR lpFileName, LPCSTR lpProcName)
{
	PVOID insthook;
	HINSTANCE hinst = NULL;
	hinst = LoadLibrary(lpFileName);//����dll�ļ�
	if (hinst == NULL)
	{
		return NULL;
	}
	insthook = GetProcAddress(hinst, lpProcName);//��ȡ������ַ
	if (insthook == NULL)
	{
		return NULL;
	}
	return insthook;
}
int COutGoingFileToolDlg::CompressFile(const char* comSavewhere,const char* needCom)//ѹ���ļ�
{
	char comCmd[2048] = { 0 };
	sprintf_s(comCmd, "E:\\fileControl_Hook\\HookProject_test\\fileHook\\OutGoingFileTool\\OutGoingFileTool\\Rar.exe a -ep %s %s", comSavewhere, needCom);
	int ret = WinExec(comCmd, SW_HIDE);
	return ret;
}
int COutGoingFileToolDlg::UncompreFile(const char* uncomTowhere, const char* needUncom)//��ѹ�ļ�
{
	char uncomCmd[2048] = { 0 };
	sprintf_s(uncomCmd, "E:\\fileControl_Hook\\HookProject_test\\fileHook\\OutGoingFileTool\\OutGoingFileTool\\UnRAR.exe -y -p- -u x %s %s", needUncom, uncomTowhere);
	int ret = WinExec(uncomCmd, SW_HIDE);
	free((void *)needUncom);
	return ret;
}
char* COutGoingFileToolDlg::CString2char(CString src)
{
	char *m_date;
	size_t m_length;
	m_length = src.GetLength() * 2 + 1;
	m_date = (char *)malloc(m_length);
	memset(m_date, 0, m_length);
	size_t nlength = wcslen(src.GetBuffer());
	//��ȡת����ĳ���
	int nbytes = WideCharToMultiByte(0, 0,src.GetBuffer(),nlength,  NULL, 0,NULL,NULL);
	if (nbytes>(int)m_length)   nbytes = m_length;
	// ͨ�����ϵõ��Ľ����ת��unicode �ַ�Ϊascii �ַ�
	WideCharToMultiByte(0, 0, src.GetBuffer(),nlength, m_date,nbytes,NULL,NULL);
	return m_date;
}