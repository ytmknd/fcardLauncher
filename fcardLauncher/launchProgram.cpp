
#include "stdafx.h"
#include <Windows.h>
#include <shlobj.h>
#include "launchProgram.h"

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

