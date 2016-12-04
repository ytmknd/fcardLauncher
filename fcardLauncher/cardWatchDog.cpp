
#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <shlobj.h>
#include "../felicalib/src/include/felicalib.h"
#include "cardWatchDog.h"
#include "launchProgram.h"

INT_PTR CALLBACK    Caution(HWND, UINT, WPARAM, LPARAM);

pasori *pPasori;	// pasori ポインタ
bool bCardReaderEnabled = false;

static int state = ST_START_WATCHING;

#define COUNT_LIMIT_MISLAYING (5*6)
static int counter_mislaying = 0;

#define COUNT_LIMIT_PROGRAM_STARTUP (3)
static int counter_program_startup = 0;

//
//  関数: openCardReader()
//
//  目的: カードリーダーを初期化し、カードの読み取りが可能な状態にします。
//
bool openCardReader()
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
//  関数: closeCardReader()
//
//  目的:  カードリーダーの使用を終了しリソースを開放します。
//
bool closeCardReader()
{
	if (pPasori) {
		pasori_close(pPasori);
	}

	return true;
}

//
//  関数: IsMislayingFelicaCardOnReader()
//
//  目的: カードしまい忘れ判定
//
bool IsMislayingFelicaCardOnReader()
{
	counter_mislaying += 1;

	if (counter_mislaying >= COUNT_LIMIT_MISLAYING) {
		return true;
	}
	else {
		return false;
	}
}

//
//  関数: readIDmFromFelicaCard()
//
//  目的: 
//
bool readIDmFromFelicaCard() {
	felica *f; // felica ポインタ
	uint8 data[READ_BLOCK_LENGTH + 1]; //readしたデータの受け口 
	bool ret = false;

	f = felica_polling(pPasori, SYSTEM_CODE, 0, 0);

	if (!f) {
		OutputDebugString(L"not found.\n");
		counter_mislaying = 0;
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

//
//  関数: WatchDogStatusUpdate()
//
//  目的: 
//
bool WatchDogStatusUpdate(int event, HWND hwnd)
{
	extern HINSTANCE hInst;

	switch (event)
	{
	case EV_TIMER_UPDATE:
		switch (state)
		{
		case ST_START_WATCHING:
			if (openCardReader()) {
				bCardReaderEnabled = true;
				state = ST_WAITING_CARD;
			}
			else {
				bCardReaderEnabled = false;
				state = ST_WAITING_READER_READY;
			}
			break;
		case ST_WAITING_PROGRAM_STARTUP:
			counter_program_startup += 1;
			if (counter_program_startup >= COUNT_LIMIT_PROGRAM_STARTUP) {
				state = ST_WAITING_CARD;
				counter_program_startup = 0;
			}
			break;
		default:
			if (bCardReaderEnabled) {
				if (readIDmFromFelicaCard()) {
					
					DFC dfc = {0,0};
					// Program起動
					launchProgram(dfc);
					counter_program_startup = 0;
					state = ST_WAITING_PROGRAM_STARTUP;

					// カードしまい忘れ警告
					if (IsMislayingFelicaCardOnReader()) {
						if (FindWindow(NULL, L"Caution!") == NULL) {
							DialogBox(hInst, MAKEINTRESOURCE(IDD_CAUTIONDIALOG), hwnd, Caution);
						}
					}
				}
			}
		}
		break;
	case EV_NFC_READER_ENABLED:
		if (openCardReader()) {
			bCardReaderEnabled = true;
			state = ST_WAITING_CARD;
		}
		break;
	case EV_NFC_READER_DISABLED:
		closeCardReader();
		bCardReaderEnabled = false;
		state = ST_WAITING_READER_READY;
		break;
	default:
		break;
	}

	return true;
}

//
//  関数: CALLBACK Caution
//
//  目的: カードしまい忘れ警告ダイアログのメッセージ ハンドラー
//
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
