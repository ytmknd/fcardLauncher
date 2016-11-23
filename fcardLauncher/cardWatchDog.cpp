
#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <shlobj.h>
#include "../felicalib/src/include/felicalib.h"
#include "cardWatchDog.h"
#include "launchProgram.h"

INT_PTR CALLBACK    Caution(HWND, UINT, WPARAM, LPARAM);

pasori *pPasori;	// pasori �|�C���^
bool bPasoriEnabled = false;

static int counter = 0;
#define COUNT_LIMIT (5*3)

//
//  �֐�: openFelicaReader()
//
//  �ړI: Felica�J�[�h���[�_�[�����������A�J�[�h�̓ǂݎ�肪�\�ȏ�Ԃɂ��܂��B
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
//  �֐�: closeFelicaReader()
//
//  �ړI:  Felica�J�[�h���[�_�[�̎g�p���I�������\�[�X���J�����܂��B
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

bool felicaCardStatusUpdate(int event, HWND hwnd)
{
	extern HINSTANCE hInst;

	switch (event)
	{
	case TIMER_UPDATE:
		if (bPasoriEnabled) {
			if (readIDmFromFelicaCard()) {
				// Program�N��
				execProgram(0000);

				// �J�[�h�d���Y��x��
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

// �J�[�h�d���Y��x���_�C�A���O�̃��b�Z�[�W �n���h���[�ł��B
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
