
#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <shlobj.h>
#include "../felicalib/src/include/felicalib.h"
#include "cardWatchDog.h"
#include "launchProgram.h"

INT_PTR CALLBACK    Caution(HWND, UINT, WPARAM, LPARAM);

pasori *pPasori;	// pasori ポインタ
bool bPasoriEnabled = false;

static int counter = 0;
#define COUNT_LIMIT (5*3)

//
//  関数: openFelicaReader()
//
//  目的: Felicaカードリーダーを初期化し、カードの読み取りが可能な状態にします。
//
bool openFelicaReader()
{
	pPasori = pasori_open(NULL);
	if (!pPasori) return false;

	if (pasori_init(pPasori) != 0) {
		if (pPasori) {
			pasori_close(pPasori);
			pPasori = NULL;
		}
		return false;
	}

	return true;
}

//
//  関数: closeFelicaReader()
//
//  目的:  Felicaカードリーダーの使用を終了しリソースを開放します。
//
bool closeFelicaReader()
{
	if (pPasori) {
		pasori_close(pPasori);
	}

	return true;
}

bool isMislayingFelicaCardOnReader()
{
	counter += 1;

	if (counter >= COUNT_LIMIT) {
		return true;
	}
	else {
		return false;
	}
}

bool readIDmFromFelicaCard() {
	felica *f; // felica ポインタ
	uint8 data[READ_BLOCK_LENGTH + 1]; //readしたデータの受け口 
	bool ret = false;

	f = felica_polling(pPasori, SYSTEM_CODE, 0, 0);

	if (!f) {
		OutputDebugString(L"not found.\n");
		counter = 0;
		ret = false;
	}
	else {
		felica_getidm(f, data);
		OutputDebugString(L"found.\n");
		felica_free(f);
		ret = true;
	}

	return ret;
}

bool felicaCardStatusUpdate(int event, HWND hwnd)
{
	extern HINSTANCE hInst;

	switch (event)
	{
	case TIMER_UPDATE:
		if (bPasoriEnabled) {
			if (readIDmFromFelicaCard()) {
				// Program起動
				execProgram(0000);

				// カード仕舞忘れ警告
				if (isMislayingFelicaCardOnReader()) {
					if (FindWindow(NULL, L"Caution!") == NULL) {
						DialogBox(hInst, MAKEINTRESOURCE(IDD_CAUTIONDIALOG), hwnd, Caution);
					}
				}
			}
		}
		break;
	case NFC_READER_ENABLED:
		if ( openFelicaReader() ) bPasoriEnabled = true;
		break;
	case NFC_READER_DISABLED:
		closeFelicaReader();
		bPasoriEnabled = false;
		break;
	default:
		break;
	}

	return true;
}

// カード仕舞忘れ警告ダイアログのメッセージ ハンドラーです。
INT_PTR CALLBACK Caution(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
