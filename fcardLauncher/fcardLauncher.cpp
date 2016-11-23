// fcardLauncher.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <Windows.h>
#include <devguid.h>
#include <Dbt.h>
#include <shellapi.h>
#include "../felicalib/src/include/felicalib.h"
#include "fcardLauncher.h"
#include "intervalTimer.h"
#include "cardWatchDog.h"

#define MAX_LOADSTRING 100

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;
const GUID GUID_DEVINTERFACE_NFP = { 0xFB3842CD, 0x9E2A, 0x4F83, {0x8F, 0xCC, 0x4B, 0x07, 0x61, 0x13, 0x9A, 0xE9} }; /* {FB3842CD-9E2A-4F83-8FCC-4B0761139AE9} */

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化しています。
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FCARDLAUNCHER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーションの初期化を実行します:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FCARDLAUNCHER));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FCARDLAUNCHER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FCARDLAUNCHER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   //UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: AddNotificationIcon()
//
//  目的: 通知領域にインジケーターアイコンを表示します。
//
BOOL AddNotificationIcon(HWND hwnd)
{
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.hWnd = hwnd;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP | NIF_GUID;
	nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
	LoadString(hInst, IDS_TOOLTIP, nid.szTip, ARRAYSIZE(nid.szTip));
	Shell_NotifyIcon(NIM_ADD, &nid);

	nid.uVersion = NOTIFYICON_VERSION_4;
	return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

//
//  関数: DeleteNotificationIcon()
//
//  目的: 通知領域からインジケーターアイコンを削除します。
//
BOOL DeleteNotificationIcon(HWND hwnd)
{
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.hWnd = hwnd;
	nid.uFlags = NIF_ICON;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

//
//  関数: ShowContextMenu()
//
//  目的: インジケーターアイコン上で右クリックした時に表示されるメニューの処理を行います。
//
void ShowContextMenu(HWND hwnd, POINT pt)
{
	HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_CONTEXTMENU));
	if (hMenu)
	{
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (hSubMenu)
		{
			SetForegroundWindow(hwnd);

			UINT uFlags = TPM_RIGHTBUTTON;
			if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
			{
				uFlags |= TPM_RIGHTALIGN;
			}
			else
			{
				uFlags |= TPM_LEFTALIGN;
			}

			TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hwnd, NULL);
		}
		DestroyMenu(hMenu);
	}
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウの描画
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
		if (!AddNotificationIcon(hWnd))
		{
			MessageBox(hWnd, L"Error adding icon", L"Error", MB_OK);
			return -1;
		}
		if (!StartIntervalTimer(hWnd))
		{
			MessageBox(hWnd, L"Error Start Timer", L"Error", MB_OK);
			return -1;
		}
		{
			DEV_BROADCAST_DEVICEINTERFACE *pFilterData =
				(DEV_BROADCAST_DEVICEINTERFACE*)
				_alloca(sizeof(DEV_BROADCAST_DEVICEINTERFACE));
			ZeroMemory(pFilterData, sizeof(DEV_BROADCAST_DEVICEINTERFACE));

			pFilterData->dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
			pFilterData->dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			pFilterData->dbcc_classguid = GUID_DEVINTERFACE_NFP;

			if (!RegisterDeviceNotification(hWnd, pFilterData, DEVICE_NOTIFY_WINDOW_HANDLE)) {
				return -1;
			}
		}
		if (!openFelicaReader())
		{
			MessageBox(hWnd, L"Error Felica Reader initialize", L"Error", MB_OK);
			felicaCardStatusUpdate(NFC_READER_DISABLED, hWnd);
			//return -1;
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WMAPP_NOTIFYCALLBACK:
		switch (LOWORD(lParam))
		{
		case WM_CONTEXTMENU:
		{
			POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
			ShowContextMenu(hWnd, pt);
		}
		break;
		default:
			;
		}
		break;
	case WM_TIMER:
		ProcIntervalTimerNortification(hWnd);
		break;
	case WM_DEVICECHANGE:
		{	
			DEV_BROADCAST_HDR* pHdr;
			DEV_BROADCAST_DEVICEINTERFACE *pSt;
			if (lParam) {
				pHdr = (DEV_BROADCAST_HDR*)lParam;
				if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
					pSt = (DEV_BROADCAST_DEVICEINTERFACE*)lParam;
					switch (wParam)
					{
					case DBT_DEVICEARRIVAL:
						OutputDebugString(L"Attached");
						felicaCardStatusUpdate(NFC_READER_ENABLED, hWnd);
						break;
					case DBT_DEVICEREMOVECOMPLETE:
						OutputDebugString(L"Detached");
						felicaCardStatusUpdate(NFC_READER_DISABLED, hWnd);
						break;
					default:
						break;
					}
				}
			}
			break;
		}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		StopIntervalTimer(hWnd);
		DeleteNotificationIcon(hWnd);
		closeFelicaReader();
		PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
