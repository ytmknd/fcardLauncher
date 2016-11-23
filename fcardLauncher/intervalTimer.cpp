
#include "stdafx.h"
#include <shellapi.h>

#include "intervalTimer.h"
#include "cardWatchDog.h"

//
//  �֐�: StartIntervalTimer()
//
//  �ړI: �Ăяo�������̏�������A�^�C�}�[���J�n���܂��B
//
BOOL StartIntervalTimer(HWND hwnd)
{
	if (SetTimer(hwnd, 1, TIMER_INTERVAL, NULL))
	{
		return true;
	}
	else {
		return false;
	}
}

//
//  �֐�: StopIntervalTimer()
//
//  �ړI: �Ăяo�������̏I��������A�^�C�}�[���~���܂��B
//
BOOL StopIntervalTimer(HWND hwnd)
{
	return KillTimer(hwnd, 1);
}

//
//  �֐�: ProcIntervalTimerNortification()
//
//  �ړI: �^�C�}�[����̒ʒm�����������ɏ������Ăяo���܂��B
//
BOOL ProcIntervalTimerNortification(HWND hwnd)
{
	felicaCardStatusUpdate(TIMER_UPDATE, hwnd);

	return true;
}
