
#include "stdafx.h"
#include <shellapi.h>

#include "intervalTimer.h"
#include "cardWatchDog.h"

//
//  関数: StartIntervalTimer()
//
//  目的: 呼び出し処理の初期化後、タイマーを開始します。
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
//  関数: StopIntervalTimer()
//
//  目的: 呼び出し処理の終了処理後、タイマーを停止します。
//
BOOL StopIntervalTimer(HWND hwnd)
{
	return KillTimer(hwnd, 1);
}

//
//  関数: ProcIntervalTimerNortification()
//
//  目的: タイマーからの通知をきっかけに処理を呼び出します。
//
BOOL ProcIntervalTimerNortification(HWND hwnd)
{
	felicaCardStatusUpdate(TIMER_UPDATE, hwnd);

	return true;
}
