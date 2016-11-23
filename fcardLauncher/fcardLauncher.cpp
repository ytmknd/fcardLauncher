// fcardLauncher.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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

// �O���[�o���ϐ�:
HINSTANCE hInst;                                // ���݂̃C���^�[�t�F�C�X
WCHAR szTitle[MAX_LOADSTRING];                  // �^�C�g�� �o�[�̃e�L�X�g
WCHAR szWindowClass[MAX_LOADSTRING];            // ���C�� �E�B���h�E �N���X��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
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

    // TODO: �����ɃR�[�h��}�����Ă��������B

    // �O���[�o������������������Ă��܂��B
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FCARDLAUNCHER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // �A�v���P�[�V�����̏����������s���܂�:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FCARDLAUNCHER));

    MSG msg;

    // ���C�� ���b�Z�[�W ���[�v:
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
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
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
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

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
//  �֐�: AddNotificationIcon()
//
//  �ړI: �ʒm�̈�ɃC���W�P�[�^�[�A�C�R����\�����܂��B
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
//  �֐�: DeleteNotificationIcon()
//
//  �ړI: �ʒm�̈悩��C���W�P�[�^�[�A�C�R�����폜���܂��B
//
BOOL DeleteNotificationIcon(HWND hwnd)
{
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.hWnd = hwnd;
	nid.uFlags = NIF_ICON;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

//
//  �֐�: ShowContextMenu()
//
//  �ړI: �C���W�P�[�^�[�A�C�R����ŉE�N���b�N�������ɕ\������郁�j���[�̏������s���܂��B
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
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:    ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND  - �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT    - ���C�� �E�B���h�E�̕`��
//  WM_DESTROY  - ���~���b�Z�[�W��\�����Ė߂�
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
            // �I�����ꂽ���j���[�̉��:
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
            // TODO: HDC ���g�p����`��R�[�h�������ɒǉ����Ă�������...
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

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
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
