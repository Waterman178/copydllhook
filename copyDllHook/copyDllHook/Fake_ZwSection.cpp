#include "stdafx.h"
#include "copyDllHook.h"
#include "ntstatus.h"
#include <string>
#include <mutex>
#include "../../OutGoingFileTool/OutGoingFileTool/FIlestruct.h"
typedef struct _mySectionObj {
	HANDLE m_SectionHandle;
	ACCESS_MASK m_DesiredAccess;
	ULONG m_AllocationAttributes;
	ULONG m_SectionPageProtection;
	HANDLE m_FileHandle;
}mySectionObj, *pmySectionObj;
std::list<mySectionObj> Sectionlist;
std::list<mySectionObj>::iterator map_Secite;
//CreateObjectAttrib(std::wstring wstrMapName) {
   //	 srand(time(NULL));
   //	 int i;
   //	 for (i = 0; i < len; ++i)
   //	 {
   //		 switch ((rand() % 3))
   //		 {
   //		 case 1:
   //			 str[i] = 'A' + rand() % 26;
   //			 break;
   //		 case 2:
   //			 str[i] = 'a' + rand() % 26;
   //			 break;
   //		 default:
   //			 str[i] = '0' + rand() % 10;
   //			 break;
   //		 }
   //	 }
   //	 str[++i] = '\0';
   //	 _tcscpy(strTemp2, strTemp.c_str());
//}
NTSTATUS
NTAPI
HookZwCreateSection(
	__out PHANDLE SectionHandle,
	__in ACCESS_MASK DesiredAccess,
	__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
	__in_opt PLARGE_INTEGER MaximumSize,
	__in ULONG SectionPageProtection,
	__in ULONG AllocationAttributes,
	__in_opt HANDLE FileHandle)
{
	bool bRet = FALSE;
	NTSTATUS ntStatus;
	ULONG Attributes = 0;
	UNICODE_STRING secName;
	HANDLE RootDirectory = NULL;
	PVOID SecurityDescriptor = NULL;
	OBJECT_ATTRIBUTES NewObjectAttributes;
	//FileHandleRelationNode robj;
	char* pOldViewBase = NULL;
	char* pNewViewBase = NULL;
	ACCESS_MASK OldAccess = STANDARD_RIGHTS_REQUIRED | SECTION_QUERY | SECTION_MAP_READ | SECTION_MAP_WRITE;
	SIZE_T SizeView = 0;
	//LARGE_INTEGER ulFileSize = {0};
	HANDLE hNewSection = INVALID_HANDLE_VALUE;
	HANDLE hOldSection = INVALID_HANDLE_VALUE;
	HANDLE hTmpSection = INVALID_HANDLE_VALUE;
	//char cArrDGKey[HeaderKeyLength] = {0}; //解密key
	IO_STATUS_BLOCK iostatus;
	std::wstring wstrMapName = L"123456s";
	FILE_STANDARD_INFORMATION fsi;
	LARGE_INTEGER maxSize;
	maxSize.HighPart = 0;
	maxSize.LowPart = 9;
	std::mutex mutexObj;
	//CRc4 rc4Obj;
	//FILE_RESULT fileResult = OpenError;
	//bRet = m_handleList.Find(robj, FileHandle);
	if (!m_handleList.empty())
	{
		mutexObj.lock();
		for (handleListNode = m_handleList.begin(); handleListNode != m_handleList.end(); handleListNode++)
		{
			if (FileHandle == handleListNode->FileHandle)
			{

				bRet = FileHandle;
				OutputDebugStringEx("发现目标句柄:%d", FileHandle);
				mutexObj.unlock();
				goto dealSection;
			}
			else
			{
				//OutputDebugStringEx("没有发现");
				ntStatus = orgZwCreateSection(
					__out  SectionHandle,
					__in  DesiredAccess,
					__in_opt  ObjectAttributes,
					__in_opt  MaximumSize,
					__in  SectionPageProtection,
					__in  AllocationAttributes,
					__in_opt  FileHandle);
				return ntStatus;
			}
		}
		mutexObj.unlock();
	dealSection:
		if (bRet /*&& robj.m_FileInfo.bReadDecrypt*/)
		{
			if (DesiredAccess & SECTION_MAP_WRITE)
			{
				//robj.m_FileInfo.bWriteFlag = true;
				//m_handleList.Update(robj);
			}
			if (/*robj.m_FileInfo.bEncryptFile
				&& */(SectionPageProtection & ~PAGE_NOACCESS))
			{
				//if (IsAllow(robj) != OpenSucess /*|| (fileResult == OpenOldFileType)*/) // 密级、企业码、加密槽等判断
				//{
				//	SetLastError(ERROR_ACCESS_DENIED);
				//	*SectionHandle = INVALID_HANDLE_VALUE;
				//	//OutputLogMsg(LOGLEVEL_ERROR, L"[%s] ACCESS_DENIED", __FUNCTIONW__);
				//	return STATUS_ACCESS_DENIED;
				//}
				//MapSectionNode sectionObj(DesiredAccess, SectionPageProtection, AllocationAttributes, FileHandle);
				pmySectionObj SectionObj = new mySectionObj();
				SectionObj->m_DesiredAccess = DesiredAccess;
				SectionObj->m_SectionPageProtection = SectionPageProtection;
				SectionObj->m_AllocationAttributes = AllocationAttributes;
				SectionObj->m_FileHandle = FileHandle;
				if (!Sectionlist.empty())
				{
					for (map_Secite = Sectionlist.begin(); map_Secite != Sectionlist.end(); map_Secite++)
					{
						if (map_Secite->m_SectionHandle == hTmpSection)
						{
							*SectionHandle = hTmpSection;
							OutputDebugStringEx("找到临时的hTmpSection");
							//::MessageBox(NULL, "1111", "dsadsa", MB_YESNO | MB_ICONEXCLAMATION);
							return STATUS_SUCCESS;
						}

					}
				}
				ntStatus = m_pfnOriginalZwQueryInformationFile(FileHandle,
					&iostatus,
					&fsi,
					sizeof(FILE_STANDARD_INFORMATION),
					FileStandardInformation);
				if (fsi.EndOfFile.QuadPart != 0)
				{
					//OutputDebugStringEx("获取文件长度:%08x", fsi.EndOfFile.QuadPart);
					if (fsi.EndOfFile.QuadPart < sizeof(RjFileSrtuct) + 1)
					{
						fsi.EndOfFile.QuadPart = sizeof(RjFileSrtuct) + 1;
					}
					ntStatus = orgZwCreateSection(
						__out  &hOldSection,
						__in  DesiredAccess,
						__in_opt  ObjectAttributes,
						__in_opt  &fsi.AllocationSize,
						__in  PAGE_READWRITE,
						__in  SEC_COMMIT,
						__in_opt  0);

					if (!NT_SUCCESS(ntStatus))
					{
						return ntStatus;
					}
					//OutputDebugStringEx("第一次m_pfnOriginalZwMapViewOfSection");
					ntStatus = m_pfnOriginalZwMapViewOfSection(
						__in hOldSection,
						__in NtCurrentProcess,
						__inout(PVOID*)&pOldViewBase,
						__in 0,
						__in 0,
						__inout_opt 0,
						__inout &SizeView,
						__in ViewShare,
						__in 0,
						__in PAGE_READWRITE);

					if (ntStatus == STATUS_ACCESS_DENIED)
					{
						m_pfnOriginalZwClose(hOldSection);
						ntStatus = orgZwCreateSection(
							__out  &hOldSection,
							__in  OldAccess,
							__in_opt  ObjectAttributes,
							__in_opt  &fsi.AllocationSize,
							__in  PAGE_READWRITE,
							__in  SEC_COMMIT,
							__in_opt  0);

						if (!NT_SUCCESS(ntStatus))
						{
							//OutputDebugStringEx("m_pfnOriginalZwMapViewOfSection -->orgZwCreateSection失败");
						}
						//OutputDebugStringEx("第二次m_pfnOriginalZwMapViewOfSection");
						ntStatus = m_pfnOriginalZwMapViewOfSection(
							__in hOldSection,
							__in  NtCurrentProcess,
							__inout(PVOID*)&pOldViewBase,
							__in 0,
							__in 0,
							__inout_opt 0,
							__inout &SizeView,
							__in ViewShare,
							__in 0,
							__in PAGE_READWRITE);
						//OutputDebugStringEx("org File Section length:%d", SizeView);
						if (!NT_SUCCESS(ntStatus))
						{
							m_pfnOriginalZwClose(hOldSection);
							//OutputDebugStringEx("m_pfnOriginalZwMapViewOfSection失败");
							return ntStatus;
						}
					}
					else if (!NT_SUCCESS(ntStatus))
					{
						m_pfnOriginalZwClose(hOldSection);
						return ntStatus;
					}
					if (ObjectAttributes != NULL)
					{
						if (ObjectAttributes->Attributes)
						{
							Attributes = ObjectAttributes->Attributes;
							SecurityDescriptor = ObjectAttributes->SecurityDescriptor;
						}
						if (ObjectAttributes->RootDirectory)
						{
							RootDirectory = ObjectAttributes->RootDirectory;
						}
					}
					m_pfnOriginalRtlInitUnicodeString(&secName, wstrMapName.c_str());
					InitializeObjectAttributes(&NewObjectAttributes,
						&secName,
						Attributes,
						RootDirectory,
						SecurityDescriptor);
					// 根据handle创建Section
					if (ObjectAttributes)
					{
						OutputDebugStringEx("ObjectAttributes为空");
					}
					ntStatus = orgZwCreateSection(
						__out  &hNewSection,
						__in  DesiredAccess,
						__in_opt(ObjectAttributes != NULL) ? &NewObjectAttributes : ObjectAttributes, // guid
						__in_opt &fsi.EndOfFile,
						__in  SectionPageProtection,
						__in  SEC_COMMIT,
						__in_opt  FileHandle);
					if (!NT_SUCCESS(ntStatus))
					{
						//OutputDebugStringEx("原先的映射失败");
						return ntStatus;
					}
					SizeView = fsi.EndOfFile.LowPart/* + g_nLenOfDGFileHeader*/;
					//OutputDebugStringEx("new File Section length:%d", SizeView);
					ntStatus = m_pfnOriginalZwMapViewOfSection(
						__in hNewSection,
						__in  NtCurrentProcess,
						__inout(PVOID*)&pNewViewBase,
						__in 0,
						__in 0,
						__inout_opt 0,
						__inout &SizeView,
						__in ViewShare,
						__in 0,
						__in SectionPageProtection/*PAGE_READONLY*/);

					if (!NT_SUCCESS(ntStatus) || pNewViewBase == NULL)
					{
						m_pfnOriginalZwClose(hNewSection);
						return ntStatus;
					}
					auto FileSize = ((pRjFileSrtuct)pNewViewBase)->length;
					//OutputDebugStringEx("org File length:%d", SizeView);
					//OutputDebugStringEx("org File content:%s", pNewViewBase);
					//OutputDebugStringEx("org File FileSize:%d", FileSize);
					//OutputDebugStringEx("pOldViewBase File content:%d", *(int*)pOldViewBase);
					//ZeroMemory(pOldViewBase, sizeof(RjFileSrtuct));
					//::MessageBox(NULL, "1111", "dsadsa", MB_YESNO | MB_ICONEXCLAMATION);
					for (int i = 0; i < FileSize; i++)
					{
						pOldViewBase[i] = pNewViewBase[i + sizeof(RjFileSrtuct) + 1];
						pOldViewBase[i] ^= 'a';
					}
					//handleListNode->m_FileInfo.bReadDecrypt = FALSE;
					//handleListNode->m_FileInfo.bEncryptFile = FALSE;
					//memcpy(pOldViewBase, pNewViewBase+sizeof(RjFileSrtuct) + 1, FileSize);

					if (pOldViewBase != NULL)
					{
						m_pfnOriginalZwUnmapViewOfSection(NtCurrentProcess, pOldViewBase);
					}

					if (pNewViewBase != NULL)
					{
						m_pfnOriginalZwUnmapViewOfSection(NtCurrentProcess, pNewViewBase);
					}
					m_pfnOriginalZwClose(hNewSection);
					*SectionHandle = hOldSection;
					SectionObj->m_SectionHandle = hOldSection;
					//robj.m_MapSection = sectionObj;
					//m_handleList.Update(robj);
					//MAPHAD_list.Add(sectionObj);
					Sectionlist.push_back(*SectionObj);
					//OutputDebugStringEx("push SectionObj Sucesss!");
					//OutputLogMsg(LOGLEVEL_INFO, L"+[%s][5-5]Handle %d OK", __FUNCTIONW__, *SectionHandle);
					return ntStatus;
				}
			}

		}
	}
	ntStatus = orgZwCreateSection(
		__out  SectionHandle,
		__in  DesiredAccess,
		__in_opt  ObjectAttributes,
		__in_opt  MaximumSize,
		__in  SectionPageProtection,
		__in  AllocationAttributes,
		__in_opt  FileHandle);
	if (!NT_SUCCESS(ntStatus))
	{
		//OutputDebugStringEx("失败%08x",ntStatus);
	}
	return ntStatus;
}