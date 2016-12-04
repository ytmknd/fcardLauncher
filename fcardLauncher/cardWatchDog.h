#pragma once

#include "resource.h"
#include "../felicalib/src/include/felicalib.h"

#define SYSTEM_CODE 0x88B4  //
#define SERVICE_CODE 0x000B //
#define READ_BLOCK_LENGTH 0x10

enum {
	ST_START_WATCHING,
	ST_WAITING_READER_READY,
	ST_WAITING_CARD,
	ST_WAITING_PROGRAM_STARTUP
};

enum {
	NOP,
	EV_TIMER_UPDATE,
	EV_NFC_READER_ENABLED,
	EV_NFC_READER_DISABLED
};

extern pasori *pPasori;	// pasori ポインタ

extern bool openCardReader();
extern bool closeCardReader();
extern bool WatchDogStatusUpdate(int event, HWND hwnd);
