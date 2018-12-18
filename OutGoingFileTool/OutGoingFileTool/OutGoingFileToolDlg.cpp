
// OutGoingFileToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OutGoingFileTool.h"
#include "OutGoingFileToolDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include <stdio.h>
#include <TlHelp32.h>

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;
#define   WM_EXITPROCESS   (WM_USER + 0x103)
BOOL IsWow64(HANDLE PROCESS)
{
	BOOL bIsWow64 = FALSE;

	//IsWow64Process is not available on all supported versions of Windows.  
	//Use GetModuleHandle to get a handle to the DLL that contains the function  
	//and GetProcAddress to get a pointer to the function if available.  

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(PROCESS, &bIsWow64))
		{
			//handle error  
		}
	}
	return bIsWow64;
}


typedef struct _UPDATASTATIC
{
	char * Dlgthis;
	SHELLEXECUTEINFO* pShExecInfo;
}UPDATASTATIC, *PUPDATASTATIC;


UPDATASTATIC updata;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
void OutputDebugStringEx(const char *strOutputString, ...){
	va_list vlArgs = NULL;
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1;
	char *strBuffer = new char[nLen];
	_vsnprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);
	delete[] strBuffer;
}
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// COutGoingFileToolDlg 对话框
COutGoingFileToolDlg::COutGoingFileToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COutGoingFileToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
COutGoingFileToolDlg::~COutGoingFileToolDlg()
{
	USES_CONVERSION;
	UnstallHook = (BOOL(WINAPI*)())LoadDllFunc(TEXT("copyDllHookx86.dll"), "EndHookMsg");
	UnstallHook();
	if (!my_Cstr.IsEmpty())
		DeleteFileW(A2CW(my_Cstr));
	HWND PWND = ::FindWindowA(NULL, "Wrenchx64");
	if (PWND)
		OutputDebugStringA("Find Wrenchx64");
	auto thradId = GetWindowThreadProcessId(PWND, NULL);
	::PostThreadMessage(thradId,WM_EXITPROCESS, 0,0);
}

void COutGoingFileToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, (CWnd&)fileList);
}

BEGIN_MESSAGE_MAP(COutGoingFileToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON1, &COutGoingFileToolDlg::OnBnClickedOpenFile)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &COutGoingFileToolDlg::OnHdnItemclickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &COutGoingFileToolDlg::OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON2, &COutGoingFileToolDlg::OnBnClickedButton2)
	ON_NOTIFY(HDN_ENDDRAG, 0, &COutGoingFileToolDlg::OnHdnEnddragList1)
	ON_MESSAGE(WM_UPDATE_STATIC, &COutGoingFileToolDlg::OnUpdateStatic)
END_MESSAGE_MAP()





// COutGoingFileToolDlg 消息处理程序


BOOL COutGoingFileToolDlg::OnInitDialog()
{
	char PROGRAM_path[255] = { 0 };
	int Ret = ::MessageBox(NULL, _T("请关闭正在打开的文档，并在查看保密文件后及时关闭此程序，以免造成误加密"), _T("提示"), MB_YESNO | MB_ICONEXCLAMATION);
	if (Ret == 7)
		exit(0);
	CDialogEx::OnInitDialog();
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	SHGetSpecialFolderPath(0, PROGRAM_path, CSIDL_PROGRAM_FILESX86, 0);
	temp_PROGRAM_pathx86 = CString(PROGRAM_path);
	temp_PROGRAM_pathx86.Replace("\\", "\\\\");
	temp_PROGRAM_pathx86 += "\\\\";
	temp_PROGRAM_pathx86 += "copyDllHookx86.dll";
	temp_PROGRAM_pathx64 = CString(PROGRAM_path);
	temp_PROGRAM_pathx64.Replace("\\", "\\\\");
	temp_PROGRAM_pathx64 += "\\\\";
	temp_PROGRAM_pathx64 += "copyDllHookx64.dll";
	//CopyFile("copyDllHookx86.dll", temp_PROGRAM_pathx86.GetBuffer(),TRUE);
	//CopyFile("copyDllHookx64.dll", temp_PROGRAM_pathx64.GetBuffer(), TRUE);

	InstallHook = (void (WINAPI*)())LoadDllFunc(TEXT("copyDllHookx86.dll"), "StartHookMsg");
	InstallHook();
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO:  在此添加额外的初始化代码
	CreateDirectory(_T(E_TMPDIR), NULL);
	fileList.InsertColumn(0, _T("文件名"), LVCFMT_CENTER, 120, 0);
	fileList.InsertColumn(1, _T("修改时间"), LVCFMT_CENTER, 120, 1);
	fileList.InsertColumn(2, _T("文件大小/KB"), LVCFMT_CENTER, 120, 2);
	fileList.InsertColumn(3, _T(""), LVCFMT_CENTER, 0,3);
	//创建临时文件夹
	int ret = CreateDirectory(_T(E_TMPDIR), 0);
	if (ret == 0) {
		printf("CreateDirectory fail(%d)\n", GetLastError());
	}
	else {
		printf("CreateDirectory -> %d\n", ret);
	}
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	si.dwFlags = STARTF_USESHOWWINDOW;  
	si.wShowWindow = FALSE;        
	BOOL bRet = CreateProcess(
		NULL,           
		(LPSTR)"OurDoorToolx64.exe",
		NULL,         
		NULL,           
		FALSE,          
		0, 
		NULL,     
		NULL,
		&si,
		&pi);
	return TRUE;
}



LRESULT COutGoingFileToolDlg::OnUpdateStatic(WPARAM wParam, LPARAM lParam)
{
	USES_CONVERSION;
	if (wParam == 0) {
		fileList.DeleteItem(0);
		DeleteFileW(A2CW(my_Cstr));
	}
	return 0;
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

void COutGoingFileToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR COutGoingFileToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//void COutGoingFileToolDlg::OnBnClickedOpenFile()
//{
//
//	////加密文件
//
//	/*FILE * TEMP = fopen("E:\\XUJYALDSKFJLSKDFJKLSDFJALSDKFJLKSD\\test001.rar", "rb+");
//	FILE * TEMP1 = fopen("E:\\XUJYALDSKFJLSKDFJKLSDFJALSDKFJLKSD\\rar001.rar", "ab+");
//
//	char buf[20] = { 0 };
//	while (fread(buf, 1, 1, TEMP)) {
//		buf[0] ^= 'a';
//		fwrite(buf, 1, 1, TEMP1);
//	}
//	char scretInfo[] = { "DSKFJLSKDF" };
//	int iflag = 0;
//	while (iflag < 10)
//	{
//		fwrite(&scretInfo[iflag], 1, 1, TEMP1);
//		iflag++;
//	}
//	fflush(TEMP1);
//	fclose(TEMP);
//	fclose(TEMP1);
//	return;*/
//
//
//	char ucomDirPath[2048] = { 0 };
//
//	//设置过滤器
//	TCHAR szFilter[] = _T("rar文件(*.rar)|*.rar|全部文件(*.*)|*.*");
//	CFileDialog dlg(TRUE, _T("doc"), _T("my"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
//	if (dlg.DoModal() == IDOK)
//	{
//		//随机生成一个文件夹名称，解压选定文件到这个目录下
//		//memcpy(ucomDirPath,GetRandLetter(),15);
//		//sprintf(ucomDirPath, "%s",GetRandLetter());
//		int ret = UncompreFile(E_TMPDIR, CString2char(dlg.GetPathName()));
//		Sleep(100);														//TODO:多文件的时候感觉还是会有问题
//		//if (ret > 31)
//		//{
//			if (SetCurrentDirectory(_T(E_TMPDIR)) == TRUE)
//			{
//				OutputDebugString(_T("find the file>>>>>>>>>>>>"));
//				CFileFind  finder;
//				CString fileInfo;
//				BOOL bWorking = finder.FindFile(_T("*.*"));
//				int line = 0;
//				CTime tmp;
//				while (bWorking)
//				{
//					bWorking = finder.FindNextFile();
//					if (line > 1)
//					{
//						OutputDebugString(_T("find the file"));
//						fileList.InsertItem(line - 2, finder.GetFileName());
//						finder.GetLastWriteTime(tmp);
//						fileInfo.Format(_T("%d/%d/%d"), tmp.GetYear(), tmp.GetMonth(), tmp.GetDay());
//						fileList.SetItemText(line - 2, 1, fileInfo);
//						fileInfo.Format(_T("%ld"), finder.GetLength());
//						fileList.SetItemText(line - 2, 2, fileInfo);
//						fileList.SetItemText(line - 2, 3, finder.GetFilePath());
//					}
//					line++;
//				}
//			}
//			else
//			{
//				return;
//			}
//		//}
//		
//
//	}
//	
//}

void COutGoingFileToolDlg::OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
	CString strLangName;//选择语言的名称字符串
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
	if (-1 != pNMListView->iItem)
	{
		//获取被选择列表的第一个子项的文本
		strLangName = fileList.GetItemText(pNMListView->iItem, 3);
		//将选择的语言显示与编辑框中
		//SetDlgItemText(IDC_LANG_SEL_EDIT,strLangName);
		ShellExecute(NULL, _T("open"), strLangName, NULL, NULL, SW_SHOWNORMAL);
	}
}

 DWORD  ThreadProc(LPVOID pParam) {
	USES_CONVERSION;
	SHELLEXECUTEINFOA ShExecInfo = *((*(UPDATASTATIC *)pParam).pShExecInfo);
	COutGoingFileToolDlg *pDlg = (COutGoingFileToolDlg*)(((UPDATASTATIC *)pParam)->Dlgthis);
	while (!WaitForSingleObject(ShExecInfo.hProcess, INFINITE))
	{
		
		::PostMessage(pDlg->m_hWnd, WM_UPDATE_STATIC, 0, 0);
		return TRUE;
	}
	return FALSE;
}
void COutGoingFileToolDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	USES_CONVERSION;
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
	char  Filepullpath[250] = { 0 };
	memcpy(Filepullpath, GetWorkDir().GetBuffer(), 250);
	//GetTempPathA(261, Filepullpath);
	my_Cstr = _T(Filepullpath);
	CString strLangName;//选择语言的名称字符串
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
	if (-1 != pNMListView->iItem)
	{
		//获取被选择列表的第一个子项的文本
		strLangName = fileList.GetItemText(pNMListView->iItem, 0);
		//TODO:解压此文件
		my_Cstr += strLangName;
		auto hFile = fopen(my_Cstr.GetBuffer(), "r");
		if (!hFile)
			::MessageBox(NULL, "error", "no find file", MB_YESNO | MB_ICONEXCLAMATION);
		fclose(hFile);
		//将选择的语言显示与编辑框中
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = my_Cstr;    //要运行的文件
		ShExecInfo.lpParameters = _T("");
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOW;
		ShExecInfo.hInstApp = NULL;
		DWORD dwId = 0L;
		auto orgdll = GetWorkDir();
		orgdll.Replace("\\", "\\\\");
		orgdll += "copyDllHookx86.dll";
		auto orgdll64 = GetWorkDir();
		orgdll64.Replace("\\", "\\\\");
		orgdll64 += "copyDllHookx64.dll";
		if (ShellExecuteEx(&ShExecInfo))
		{
			HANDLE hid = ShExecInfo.hProcess;
			DWORD dwId = ::GetProcessId(ShExecInfo.hProcess);//获取打开的另一个程序的进程ID
			/*if (!IsWow64(OpenProcess(PROCESS_TERMINATE, FALSE, dwId)))
			{
			if (InjectDll(dwId, orgdll.GetBuffer()) == -1)
			{
				::MessageBox(NULL, "注入失败", "软件提示", MB_YESNO | MB_ICONEXCLAMATION);
				return;
			}
			}
			else
			{
				if (InjectDll(dwId, orgdll64.GetBuffer()) == -1)
				{
					::MessageBox(NULL, "注入失败", "软件提示", MB_YESNO | MB_ICONEXCLAMATION);
					return;
				}
			}
			updata.Dlgthis = (CHAR*)this;
			updata.pShExecInfo = &ShExecInfo;*/
			//AfxBeginThread((AFX_THREADPROC)ThreadProc, &updata, THREAD_PRIORITY_TIME_CRITICAL);
		}
	}
}
 CString GetWorkDir()
{
	char buf[MAX_PATH];
	_fullpath(buf, ".\\", MAX_PATH);
	CString csFullPath(buf);
	return csFullPath;
}
static int InjectDll(DWORD dwProcessId, PTCHAR szDllName)
{
	if (szDllName[0] == NULL)
		return -1;
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,       
		FALSE,                                             
		dwProcessId);                                      
	if (hProcess == INVALID_HANDLE_VALUE)
		return -1;
	LPVOID pszDllName = ::VirtualAllocEx(hProcess, 
		NULL,                                    
		4096,                                 
		MEM_COMMIT,                           
		PAGE_EXECUTE_READWRITE);                
	if (NULL == pszDllName)
		return -1;
	BOOL bRet = ::WriteProcessMemory(hProcess, pszDllName,
		szDllName, MAX_PATH, NULL);
	if (NULL == bRet)
		return -1;
	HANDLE m_hInjecthread = NULL;
	m_hInjecthread = ::CreateRemoteThread(hProcess,     
		NULL,                                             
		0,                                               
		(LPTHREAD_START_ROUTINE)LoadLibrary,              
		pszDllName,                                     
		NULL,                                           
		NULL);                                          
	if (NULL == m_hInjecthread)
	{
		DWORD dwErr = GetLastError();
		return -1;
	}
	DWORD dw = WaitForSingleObject(m_hInjecthread, -1);  
	DWORD dwExitCode;
	HMODULE m_hMod;
	GetExitCodeThread(m_hInjecthread, &dwExitCode);
	m_hMod = (HMODULE)dwExitCode;
	BOOL bReturn = VirtualFreeEx(hProcess, pszDllName,
		4096, MEM_DECOMMIT);
	if (NULL == bReturn)
		return -1;
	CloseHandle(hProcess);
	hProcess = NULL;
	return 0;
}
PVOID COutGoingFileToolDlg::LoadDllFunc(LPCTSTR lpFileName, LPCSTR lpProcName)
{
	PVOID insthook;
	HINSTANCE hinst = NULL;
	hinst = LoadLibrary(lpFileName);
	if (hinst == NULL)
	{
		return NULL;
	}
	insthook = GetProcAddress(hinst, lpProcName);
	if (insthook == NULL)
	{
		return NULL;
	}
	return insthook;
}
int COutGoingFileToolDlg::CompressFile(const char* comSavewhere,const char* needCom)//压缩文件
{
	char comCmd[2048] = { 0 };
	sprintf_s(comCmd, "E:\\fileControl_Hook\\HookProject_test\\fileHook\\OutGoingFileTool\\OutGoingFileTool\\Rar.exe a -ep %s %s", comSavewhere, needCom);
	int ret = WinExec(comCmd, SW_HIDE);
	return ret;
}
int COutGoingFileToolDlg::UncompreFile(const char* uncomTowhere, const char* needUncom)//解压文件
{
	char uncomCmd[2048] = { 0 };
	sprintf_s(uncomCmd, "E:\\fileControl_Hook\\HookProject_test\\fileHook\\OutGoingFileTool\\OutGoingFileTool\\UnRAR.exe -y -p- -u x %s %s", needUncom, uncomTowhere);
	int ret = WinExec(uncomCmd, SW_HIDE);
	free((void *)needUncom);
	return ret;
}
void COutGoingFileToolDlg::OnBnClickedButton2()
{
	return;
}
void COutGoingFileToolDlg::OnHdnEnddragList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}
