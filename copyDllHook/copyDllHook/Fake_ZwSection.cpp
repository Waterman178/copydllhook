
#include "stdafx.h"
#include "copyDllHook.h"
#include "ntstatus.h"
#include <string>


NTSTATUS
NTAPI
HookZwCreateSection(__out PHANDLE SectionHandle,
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
	PVOID pOldViewBase = NULL;
	PVOID pNewViewBase = NULL;
	ACCESS_MASK OldAccess = STANDARD_RIGHTS_REQUIRED | SECTION_QUERY | SECTION_MAP_READ | SECTION_MAP_WRITE;
	SIZE_T SizeView = 0;
	//LARGE_INTEGER ulFileSize = {0};
	HANDLE hNewSection = INVALID_HANDLE_VALUE;
	HANDLE hOldSection = INVALID_HANDLE_VALUE;
	HANDLE hTmpSection = INVALID_HANDLE_VALUE;
	//char cArrDGKey[HeaderKeyLength] = {0}; //解密key
	IO_STATUS_BLOCK iostatus;
	wstring wstrMapName;
	FILE_STANDARD_INFORMATION fsi;
	//CRc4 rc4Obj;
	//FILE_RESULT fileResult = OpenError;

	//bRet = m_handleList.Find(robj, FileHandle);

	if (MAPHAD_list.empty())
	{
		for (map_ite = MAPHAD_list.begin(); map_ite != MAPHAD_list.end(); map_ite++)
		{
			if (FileHandle == *map_ite)
			{
				bRet = FileHandle;
			}
		}
	

	if (bRet /*&& robj.m_FileInfo.bReadDecrypt*/)
	{
		//if (DesiredAccess & SECTION_MAP_WRITE)
		//{
		//    robj.m_FileInfo.bWriteFlag = true;
		//    m_handleList.Update(robj);
		//}

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

			/*if (m_handleList.Find(sectionObj, hTmpSection))
			{
				*SectionHandle = hTmpSection;
				return STATUS_SUCCESS;
			}
			else
			{*/
				ntStatus = m_pfnOriginalZwQueryInformationFile(FileHandle,
					&iostatus,
					&fsi,
					sizeof(FILE_STANDARD_INFORMATION),
					FileStandardInformation);

				if (fsi.EndOfFile.QuadPart != 0)
				{
					/*if (fsi.EndOfFile.QuadPart < HeaderLength)
					{
						fsi.EndOfFile.QuadPart = robj.m_FileInfo.liFileSize.QuadPart;
					}*/
					ntStatus = m_pfnOriginalZwCreateSection(
						__out  &hOldSection,
						__in  DesiredAccess,
						__in_opt  ObjectAttributes,
						__in_opt  &fsi.EndOfFile,
						__in  PAGE_READWRITE,
						__in  SEC_COMMIT,
						__in_opt  0);

					if (!NT_SUCCESS(ntStatus))
					{
						//OutputLogMsg(LOGLEVEL_ERROR, L"\t\t-[%s][1-5]Handle %d, %s 失败0x%08x", __FUNCTIONW__, hOldSection, (ObjectAttributes != NULL)? ((ObjectAttributes->ObjectName != NULL)? ObjectAttributes->ObjectName->Buffer: L""): L"", ntStatus);

						return ntStatus;
					}

					//OutputLogMsg(LOGLEVEL_INFO, L"\t\t-[%s][1-5]Handle %d, %s, EndOfFile %I64d", __FUNCTIONW__, hOldSection, (ObjectAttributes != NULL)? ((ObjectAttributes->ObjectName != NULL)? ObjectAttributes->ObjectName->Buffer: L""): L"", fsi.EndOfFile.QuadPart);

					ntStatus = m_pfnOriginalZwMapViewOfSection(
						__in hOldSection,
						__in NtCurrentProcess,
						__inout &pOldViewBase,
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

						ntStatus = m_pfnOriginalZwCreateSection(
							__out  &hOldSection,
							__in  OldAccess,
							__in_opt  ObjectAttributes,
							__in_opt  &fsi.EndOfFile,
							__in  PAGE_READWRITE,
							__in  SEC_COMMIT,
							__in_opt  0);

						if (!NT_SUCCESS(ntStatus))
						{
							//OutputLogMsg(LOGLEVEL_ERROR, L"\t\t-[%s][1-5]Handle %d, %s 失败0x%08x", __FUNCTIONW__, hOldSection, (ObjectAttributes != NULL)? ((ObjectAttributes->ObjectName != NULL)? ObjectAttributes->ObjectName->Buffer: L""): L"", ntStatus);
						}

						ntStatus = m_pfnOriginalZwMapViewOfSection(
							__in hOldSection,
							__in  NtCurrentProcess,
							__inout &pOldViewBase,
							__in 0,
							__in 0,
							__inout_opt 0,
							__inout &SizeView,
							__in ViewShare,
							__in 0,
							__in PAGE_READWRITE);

						if (!NT_SUCCESS(ntStatus))
						{
							m_pfnOriginalZwClose(hOldSection);

							//OutputLogMsg(LOGLEVEL_ERROR, L"\t\t-[%s][2-5]Handle %d, OldViewBase:%08x 失败0x%08x", __FUNCTIONW__, hOldSection, pOldViewBase, ntStatus);

							return ntStatus;
						}

					}
					else if (!NT_SUCCESS(ntStatus))
					{
						m_pfnOriginalZwClose(hOldSection);

						//OutputLogMsg(LOGLEVEL_ERROR, L"\t\t-[%s][2-5]Handle %d, OldViewBase:%08x 失败0x%08x", __FUNCTIONW__, hOldSection, pOldViewBase, ntStatus);

						return ntStatus;
					}

					//OutputLogMsg(LOGLEVEL_INFO, L"\t\t-[%s][2-5]Handle %d, OldViewBase:%08x", __FUNCTIONW__, hOldSection, pOldViewBase);
					//随机名？


					//CreateObjectAttrib(wstrMapName);

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
					ntStatus = m_pfnOriginalZwCreateSection(
						__out  &hNewSection,
						__in  DesiredAccess,
						__in_opt(ObjectAttributes != NULL) ? &NewObjectAttributes : ObjectAttributes, // guid
						__in_opt &fsi.EndOfFile,
						__in  SectionPageProtection,
						__in  SEC_COMMIT,
						__in_opt  FileHandle);

					if (!NT_SUCCESS(ntStatus))
					{
						//OutputLogMsg(LOGLEVEL_ERROR, L"\t\t-[%s][3-5]Handle %d, %s 失败0x%08x", __FUNCTIONW__, hNewSection, (ObjectAttributes != NULL)? ((ObjectAttributes->ObjectName != NULL)? ObjectAttributes->ObjectName->Buffer: L""): L"", ntStatus);

						return ntStatus;
					}

					//OutputLogMsg(LOGLEVEL_INFO, L"\t\t-[%s][3-5]Handle %d, %s", __FUNCTIONW__, hNewSection, (ObjectAttributes != NULL)? ((ObjectAttributes->ObjectName != NULL)? ObjectAttributes->ObjectName->Buffer: L""): L"");

					SizeView = fsi.EndOfFile.LowPart/* + g_nLenOfDGFileHeader*/;
					ntStatus = m_pfnOriginalZwMapViewOfSection(
						__in hNewSection,
						__in  NtCurrentProcess,
						__inout &pNewViewBase,
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

						// OutputLogMsg(LOGLEVEL_ERROR, L"\t\t-[%s][4-5]Handle %d, NewViewBase:%08x 失败0x%08x", __FUNCTIONW__, hNewSection, pNewViewBase, ntStatus);

						return ntStatus;
					}

					//OutputLogMsg(LOGLEVEL_INFO, L"\t\t-[%s][4-5]Handle %d, NewViewBase:%08x", __FUNCTIONW__, hNewSection, pNewViewBase);
					
					//换个新的解密吧
					/*if (robj.m_FileInfo.rc4Key == NULL)
					{
						robj.m_FileInfo.rc4Key = new unsigned char[FileBuffer];
						memset(robj.m_FileInfo.rc4Key, 0, FileBuffer);
					}

					rc4Obj.DecryptMemoryRc4K(pOldViewBase,
						(char*)pNewViewBase + HeaderLength,
						0,
						(DWORD)fsi.EndOfFile.LowPart - HeaderLength,
						robj.m_FileInfo.rc4Key);*/



					if (pOldViewBase != NULL)
					{
						m_pfnOriginalZwUnmapViewOfSection(HANDLE(-1), pOldViewBase);
					}

					if (pNewViewBase != NULL)
					{
						m_pfnOriginalZwUnmapViewOfSection(HANDLE(-1), pNewViewBase);
					}

					m_pfnOriginalZwClose(hNewSection);
					*SectionHandle = hOldSection;
					//sectionObj.m_SectionHandle = hOldSection;
					//robj.m_MapSection = sectionObj;
					//m_handleList.Update(robj);
					//m_mapSectionList.Add(sectionObj);

					//OutputLogMsg(LOGLEVEL_INFO, L"+[%s][5-5]Handle %d OK", __FUNCTIONW__, *SectionHandle);

					return ntStatus;
				}

			//}
		}
	}
	}
	ntStatus = m_pfnOriginalZwCreateSection(
		__out  SectionHandle,
		__in  DesiredAccess,
		__in_opt  ObjectAttributes,
		__in_opt  MaximumSize,
		__in  SectionPageProtection,
		__in  AllocationAttributes,
		__in_opt  FileHandle);

	return ntStatus;
}