
#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <shlobj.h>
#include "../felicalib/src/include/felicalib.h"
#include "cardWatchDog.h"
#include "launchProgram.h"

INT_PTR CALLBACK    Caution(HWND, UINT, WPARAM, LPARAM);

pasori *pPasori;	// pasori �|�C���^
bool bCardReaderEnabled = false;

static int state = ST_START_WATCHING;

static int counter = 0;
#define COUNT_LIMIT (5*3)

//
//  �֐�: openCardReader()
//
//  �ړI: �J�[�h���[�_�[�����������A�J�[�h�̓ǂݎ�肪�\�ȏ�Ԃɂ��܂��B
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
//  �֐�: closeCardReader()
//
//  �ړI:  �J�[�h���[�_�[�̎g�p���I�������\�[�X���J�����܂��B
//
bool closeCardReader()
{
	if (pPasori) {
		pasori_close(pPasori);
	}

	return true;
}

//
//  �֐�: IsMislayingFelicaCardOnReader()
//
//  �ړI: �J�[�h���܂��Y�ꔻ��
//
bool IsMislayingFelicaCardOnReader()
{
	counter += 1;

	if (counter >= COUNT_LIMIT) {
		return true;
	}
	else {
		return false;
	}
}

//
//  �֐�: readIDmFromFelicaCard()
//
//  �ړI: 
//
bool readIDmFromFelicaCard() {
	felica *f; // felica �|�C���^
	uint8 data[READ_BLOCK_LENGTH + 1]; //read�����f�[�^�̎󂯌� 
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

//
//  �֐�: WatchDogStatusUpdate()
//
//  �ړI: 
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
			state = ST_WAITING_CARD;
			break;
		default:
			if (bCardReaderEnabled) {
				if (readIDmFromFelicaCard()) {
					// Program�N��
					launchProgram(0000);

					// �J�[�h���܂��Y��x��
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
//  �֐�: CALLBACK Caution
//
//  �ړI: �J�[�h���܂��Y��x���_�C�A���O�̃��b�Z�[�W �n���h���[
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
