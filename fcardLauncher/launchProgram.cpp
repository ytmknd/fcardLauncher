
#include "stdafx.h"
#include <Windows.h>
#include <shlobj.h>
#include "launchProgram.h"

const LPCWSTR REGKEYPATHBASE = L"SOFTWARE\\FCARDLAUNCHER"; // TODO

struct StEntry
{
	WCHAR *dfc;
	WCHAR path[MAX_PATH];
};
#define MAX_ENTRY 5
struct StTable {
	struct StEntry entry[MAX_ENTRY];
};
static StTable jt = {
	{
		{ L"0000", NULL },
		{NULL,NULL}
	}
};

//
//  関数: ReadRegKeyString
//
//  目的: 
//
LONG ReadRegKeyString(const WCHAR *szKeyPathSub,const WCHAR *szKeyName, WCHAR *szBuffer, DWORD* dwBufferSize)
{
	ULONG nError;
	WCHAR szKeyPath[_MAX_PATH];
	HKEY hKey;

	_snwprintf_s(szKeyPath,
		(size_t)_MAX_PATH,
		(size_t)_MAX_PATH,
		L"%s\\%s",REGKEYPATHBASE, szKeyPathSub); 

	nError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKeyPath, 0, KEY_READ, &hKey);
	if (nError != ERROR_SUCCESS) {
		return nError;
	}

	nError = RegQueryValueExW(hKey, (LPCWSTR)szKeyName, 0, NULL, (LPBYTE)szBuffer, dwBufferSize);
	if (ERROR_SUCCESS == nError)
	{
		OutputDebugStringW(szBuffer);
	}
	RegCloseKey(hKey);

	return nError;
}

//
//  関数: initlaunchProgram
//
//  目的: 
//
bool initlaunchProgram()
{
	bool retValue = false;
	int i;
	DWORD dwBufferSize = MAX_PATH;

	for (i = 0; i < MAX_ENTRY; i++) {
		if (jt.entry[i].dfc == NULL) break;
		if (ReadRegKeyString(jt.entry[i].dfc, L"path",  jt.entry[i].path, &dwBufferSize) != ERROR_SUCCESS) {
			_snwprintf_s(jt.entry[i].path,
				(size_t)_MAX_PATH,
				(size_t)_MAX_PATH,
				L"C:\\Windows\\notepad.exe"); // ダミー
		}

		retValue = true;
	}

	return retValue;
}

static bool lcl_dfc2WCHAR(DFC dfc,WCHAR *szDFCStr)
{
	_snwprintf_s(szDFCStr,
		(size_t)5,
		(size_t)5,
		L"%1X%1X%1X%1X", dfc[0], dfc[1],dfc[2],dfc[3]);

	return true;
}

//
//  関数: launchProgram
//
//  目的: 
//
bool launchProgram(DFC dfc)
{
	WCHAR *szDFCStr[5];
	WCHAR szCommadLine[MAX_PATH];
	DWORD dwCommandLineSize = sizeof(szCommadLine);
	int i;
	bool procflag = false;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));

	lcl_dfc2WCHAR(dfc, (WCHAR *)szDFCStr);

	for (i = 0; i < MAX_ENTRY; i++) {
		if (jt.entry[i].dfc == NULL) break;
		if (wcscmp(jt.entry[i].dfc, (const WCHAR *)szDFCStr) == 0) {
			if (CreateProcess(NULL, jt.entry[i].path, NULL, NULL, FALSE,
				CREATE_NEW_PROCESS_GROUP, NULL, NULL,
				&si, &pi)) {
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			procflag = true;
		}
		else {
			;
		}
	}

	return procflag;
}

