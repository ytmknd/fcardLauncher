#pragma once

#define TIMER_INTERVAL (2*1000)

extern BOOL StartIntervalTimer(HWND hwnd);
extern BOOL StopIntervalTimer(HWND hwnd);
extern BOOL ProcIntervalTimerNortification(HWND hwnd);

