
#include "stdafx.h"
#include <Windows.h>
#include <shlobj.h>
#include "launchProgram.h"

HKEY hKey;
LPCWSTR strValue = L"SOFTWARE\\FCARDLAUNCHER";

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
LONG ReadRegKeyString(const WCHAR *szKeyName, WCHAR *szBuffer, DWORD* dwBufferSize)
{
	ULONG nError;

	nError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strValue, 0, KEY_READ, &hKey);
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
		if (ReadRegKeyString(jt.entry[i].dfc, jt.entry[i].path, &dwBufferSize) != ERROR_SUCCESS) {
			_snwprintf_s(jt.entry[i].path,
				(size_t)_MAX_PATH,
				(size_t)_MAX_PATH,
				L"C:\\Windows\\notepad.exe"); // ダミー
		}

		retValue = true;
	}

	return retValue;
}

//
//  関数: launchProgram
//
//  目的: 
//
bool launchProgram(UINT16 dfc)
{
	WCHAR desktop[_MAX_PATH], commandLine[_MAX_PATH];

	SHGetSpecialFolderPath(NULL, desktop, CSIDL_DESKTOPDIRECTORY, FALSE);
	_snwprintf_s(commandLine,
		(size_t)_MAX_PATH,
		(size_t)_MAX_PATH,
		L"C:\\Windows\\notepad.exe %s\\test.txt",
		desktop);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));

	if (FindWindow(L"Notepad", NULL) == NULL) {
		if (CreateProcess(NULL, commandLine, NULL, NULL, FALSE,
						CREATE_NEW_PROCESS_GROUP, NULL, NULL,
						&si, &pi)) {
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
	}
	else {
		;
	}
	return true;
}

