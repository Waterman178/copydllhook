
// OutGoingFileToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OutGoingFileTool.h"
#include "OutGoingFileToolDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include <stdio.h>
#include <TlHelp32.h>



UPDATASTATIC updata;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
void OutputDebugStringEx(const char *strOutputString, ...)

{

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
	//卸载键盘的PRINT SCREEN 按键的控制
	//UnstallHook = (BOOL (WINAPI*)())LoadDllFunc(_T("copyDllHook.dll"), "EndHookKeyBord");
	//UnstallHook();

	////卸载全局消息钩子
	//UnstallHook = (BOOL(WINAPI*)())LoadDllFunc(_T("copyDllHook.dll"), "EndHookMsg");
	//UnstallHook();
	//关闭已打开的文件
	//::SendMessage(, WM_SYSCOMMAND, SC_CLOSE, 0);
//	TerminateProcess(hid,0);
//	//删除临时文件中解压出来的文件
//	if (SetCurrentDirectory(_T(TMPDIR)) == TRUE)
//	{
//		CFileFind  finder;
//		BOOL bWorking = finder.FindFile(_T("*.*"));
//		int line = 0;
//		CTime tmp;
//		while (bWorking)
//		{
//			bWorking = finder.FindNextFile();
//			if (line > 1)
//			{
//				HWND hwnd = ::FindWindow(NULL, finder.GetFileName());
//				DeleteFile(finder.GetFilePath());		
//			}
//			line++;
//		}
//	}
//	::UnmapViewOfFile(pBuffer);
//	::CloseHandle(hMap);
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
	int Ret = ::MessageBox(NULL, _T("请关闭正在打开的文档，并在查看保密文件后及时关闭此程序，以免造成误加密"), _T("提示"), MB_YESNO | MB_ICONEXCLAMATION);
	if (Ret == 7)
		exit(0);
	CDialogEx::OnInitDialog();

	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);

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


	fileList.InsertColumn(0, _T("文件名"), LVCFMT_CENTER, 100, 0);
	fileList.InsertColumn(1, _T("修改时间"), LVCFMT_CENTER, 100, 1);
	fileList.InsertColumn(2, _T("文件大小/KB"), LVCFMT_CENTER, 100, 2);
	fileList.InsertColumn(3, _T(""), LVCFMT_CENTER, 0,3);
	//创建临时文件夹
	int ret = CreateDirectory(_T(E_TMPDIR), 0);
	if (ret == 0) {
		printf("CreateDirectory fail(%d)\n", GetLastError());
	}
	else {
		printf("CreateDirectory -> %d\n", ret);
	}

	/*hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(int),
		_T("processMem_FUCK"));*/
	// 映射对象的一个视图，得到指向共享内存的指针，设置里面的数据
	/*pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	*(int*)pBuffer = GetCurrentProcessId();*/
	
	////键盘的PRINT SCREEN 按键的控制
	//InstallHook = (void (WINAPI*)())LoadDllFunc(_T("copyDllHook.dll"), "StartHookKeyBord");
	//InstallHook();
	//
	////全局消息钩子
	//InstallHook = (void (WINAPI*)())LoadDllFunc(_T("copyDllHook.dll"), "StartHookMsg");
	//InstallHook();
	//LoadLibraryA("copyDllHook.dll");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}



LRESULT COutGoingFileToolDlg::OnUpdateStatic(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0) {
		fileList.DeleteItem(0);
	}
	else {
		fileList.DeleteItem(wParam);
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void COutGoingFileToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
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
		DeleteFileW(A2CW(my_Cstr));
		::PostMessage(pDlg->m_hWnd, WM_UPDATE_STATIC, 0, 0);
		return TRUE;
	}
	return FALSE;
}
void COutGoingFileToolDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
	char  Filepullpath[250] = { 0 };
	GetTempPathA(261, Filepullpath);
	my_Cstr = _T(Filepullpath);
	CString strLangName;//选择语言的名称字符串
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
	if (-1 != pNMListView->iItem)
	{
		//获取被选择列表的第一个子项的文本
		strLangName = fileList.GetItemText(pNMListView->iItem, 0);
		//TODO:解压此文件
		my_Cstr += strLangName;
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
		//这里创建一个线程运行算了
		//SetFileAttributes(str, FILE_ATTRIBUTE_READONLY);
		auto orgdll = GetWorkDir();
		orgdll.Replace("\\", "\\\\");
		orgdll += _T("copyDllHook.dll");
		if (ShellExecuteEx(&ShExecInfo))
		{
			HANDLE hid = ShExecInfo.hProcess;
			DWORD dwId = ::GetProcessId(ShExecInfo.hProcess);//获取打开的另一个程序的进程ID
			if (InjectDll(dwId, orgdll.GetBuffer()) == -1)
			{
				::MessageBox(NULL, "软件提示", "注入失败", MB_YESNO | MB_ICONEXCLAMATION);
				return;
			}
			updata.Dlgthis = (CHAR*)this;
			updata.pShExecInfo = &ShExecInfo;
			AfxBeginThread((AFX_THREADPROC)ThreadProc, &updata, THREAD_PRIORITY_TIME_CRITICAL);
		}
	}
}
static CString GetWorkDir()
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
	//提高权限相关操作  
	//EnablePrivilege(TRUE);
	//1. 打开进程  
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,   //打开进程权限  
		FALSE,                                              //是否可继承   
		dwProcessId);                                       //进程ID  

	if (hProcess == INVALID_HANDLE_VALUE)
		return -1;

	//2. 在远程进程中申请空间  
	LPVOID pszDllName = ::VirtualAllocEx(hProcess, //远程进程句柄  
		NULL,                                  //建议开始地址  
		4096,                                  //分配空间大小  
		MEM_COMMIT,                            //空间初始化全0  
		PAGE_EXECUTE_READWRITE);               //空间权限  

	if (NULL == pszDllName)
	{
		return -1;
	}

	//3. 向远程进程中写入数据  
	BOOL bRet = ::WriteProcessMemory(hProcess, pszDllName,
		szDllName, MAX_PATH, NULL);

	if (NULL == bRet)
	{
		return -1;
	}

	//4. 在远程进程中创建远程线程 
	HANDLE m_hInjecthread = NULL;
	m_hInjecthread = ::CreateRemoteThread(hProcess,      //远程进程句柄  
		NULL,                                            //安全属性  
		0,                                               //栈大小  
		(LPTHREAD_START_ROUTINE)LoadLibrary,             //进程处理函数      
		pszDllName,                                      //传入参数  
		NULL,                                            //默认创建后的状态  
		NULL);                                           //线程ID  

	if (NULL == m_hInjecthread)
	{
		DWORD dwErr = GetLastError();
		return -1;
	}

	//5. 等待线程结束返回  
	DWORD dw = WaitForSingleObject(m_hInjecthread, -1);
	//6. 获取线程退出码,即LoadLibrary的返回值，即dll的首地址  
	DWORD dwExitCode;
	HMODULE m_hMod;
	GetExitCodeThread(m_hInjecthread, &dwExitCode);
	m_hMod = (HMODULE)dwExitCode;

	//7. 释放空间  
	BOOL bReturn = VirtualFreeEx(hProcess, pszDllName,
		4096, MEM_DECOMMIT);

	if (NULL == bReturn)
	{
		return -1;
	}

	CloseHandle(hProcess);
	hProcess = NULL;
	//恢复权限相关操作  
	//EnablePrivilege(FALSE);

	return 0;
}
PVOID COutGoingFileToolDlg::LoadDllFunc(LPCTSTR lpFileName, LPCSTR lpProcName)
{
	PVOID insthook;
	HINSTANCE hinst = NULL;
	hinst = LoadLibrary(lpFileName);//加载dll文件
	if (hinst == NULL)
	{
		return NULL;
	}
	insthook = GetProcAddress(hinst, lpProcName);//获取函数地址
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
//char* COutGoingFileToolDlg::CString2char(CString src)
//{
//	char *m_date;
//	size_t m_length;
//	m_length = src.GetLength() * 2 + 1;
//	m_date = (char *)malloc(m_length);
//	memset(m_date, 0, m_length);
//	size_t nlength = wcslen(src.GetBuffer());
//	//获取转换后的长度
//	int nbytes = WideCharToMultiByte(0, 0,src.GetBuffer(),nlength,  NULL, 0,NULL,NULL);
//	if (nbytes>(int)m_length)   nbytes = m_length;
//	// 通过以上得到的结果，转换unicode 字符为ascii 字符
//	WideCharToMultiByte(0, 0, src.GetBuffer(),nlength, m_date,nbytes,NULL,NULL);
//	return m_date;
//}
void COutGoingFileToolDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	//char   Ext[250];
	//char  pBuffer[250];
	//INT eof = -1;
	//encryptInfo = std::shared_ptr<rjFileInfo>(new rjFileInfo());
	//memcpy(encryptInfo->encryptHead.FileHeadName, FileName, sizeof(FileName));
	//encryptInfo->encryptHead.onlyread = 1;
	//encryptInfo->encryptHead.forbidensaveas = 1;
	//FILE * TEMP = fopen("C:\\Users\\Administrator\\Desktop\\121.docx", "rb+");
	//FILE * TEMP1 = fopen("C:\\Users\\Administrator\\Desktop\\121.rjs", "wb+");
	//_splitpath_s("C:\\Users\\Administrator\\Desktop\\121.docx", NULL, 0, NULL, 0, pBuffer, _MAX_FNAME, Ext, _MAX_FNAME);// 得到文件名
	//strcat(pBuffer, Ext); //文件名衔接个后缀名
	//fseek(TEMP, 0, SEEK_END);   //指针：移动到文件尾部
	//encryptInfo->encryptHead.length = ftell(TEMP); //获取文件大小
	//memcpy(encryptInfo->encryptHead.FileSrcName, pBuffer, 60);//填写原文件名
	//encryptInfo->encryptHead.Count = 1000; //文件使用次数
	//size_t iflag = 0;
	//size_t len = sizeof(RjFileSrtuct);
	////从结构体头开始复制，已经是1字节对齐了
	//while (iflag <= len)
	//{
	//	fwrite(encryptInfo->encryptHead.FileHeadName+iflag, 1, 1, TEMP1);//这个encryptInfo->encryptHead.FileHeadName指针写法不太规范,一般懂汇编和C的编程设计者容易理解，也没用原始指针。
	//	iflag++;
	//	fflush(TEMP1);
	//}
	//char*  buf = new char[encryptInfo->encryptHead.length];
	//ZeroMemory(buf, (int)encryptInfo->encryptHead.length);
	//fseek(TEMP, 0, SEEK_SET);//移动到头部
	//while (fread(buf, 1, 1, TEMP)) {
	//	buf[0] ^= 'a';
	//	fseek(TEMP1, 0, SEEK_END);
	//	fwrite(buf, 1, 1, TEMP1);
	//	fflush(TEMP1);
	//}
	//fseek(TEMP1, 0, SEEK_END);
	//fclose(TEMP);
	//fclose(TEMP1);
	//delete buf;
	return;
}
void COutGoingFileToolDlg::OnHdnEnddragList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}
