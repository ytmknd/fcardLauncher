
#include "stdafx.h"
#include <Windows.h>
#include <shlobj.h>
#include "launchProgram.h"

bool execProgram(UINT16 dfc)
{
	WCHAR desktop[_MAX_PATH], commandLine[_MAX_PATH];

	/* get felica.dll path */
	SHGetSpecialFolderPath(NULL, desktop, CSIDL_DESKTOPDIRECTORY, FALSE);
	_snwprintf_s(commandLine,
		(size_t)_MAX_PATH,
		(size_t)_MAX_PATH,
		L"C:\\Windows\\notepad.exe %s\\test.txt",
		desktop);

	STARTUPINFO si;// スタートアップ情報
	PROCESS_INFORMATION pi;// プロセス情報
						   //STARTUPINFO 構造体の内容を取得 
	ZeroMemory(&si, sizeof(si));

	if (FindWindow(L"Notepad", NULL) == NULL) {
		int ret = 0;
		ret = CreateProcess(
			NULL,					// 実行可能モジュールの名前
			commandLine,	// コマンドラインの文字列
			NULL,					// セキュリティ記述子
			NULL,					// セキュリティ記述子
			FALSE,					// ハンドルの継承オプション
			CREATE_NEW_PROCESS_GROUP,					// 作成のフラグ 
														//CREATE_NEW_PROCESS_GROUP	: 新たなプロセス
			NULL,					// 新しい環境ブロック
			NULL,					// カレントディレクトリの名前
			&si,					// スタートアップ情報
			&pi					// プロセス情報
		);
		if (ret) {
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
	}
	else {
		;
	}
	return true;
}

