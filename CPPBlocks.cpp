#include "framework.h"
#include "resource.h"
#include "Simulation.h"

#define MAX_LOADSTRING 100

HWND g_hWnd = (HWND)(INVALID_HANDLE_VALUE);
HDC g_hDC = (HDC)(INVALID_HANDLE_VALUE);
RECT g_rect = { 0, 0, 0, 0 };
HINSTANCE hInst;                                // current instance
CHAR szTitle[MAX_LOADSTRING];                  // The title bar text
CHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

game::mouse mouse;

void OnTimer(HWND hWnd, UINT uint, UINT_PTR ptr, DWORD word);

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPSTR    lpCmdLine,
					 _In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CPPBLOCKS, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	game::Simulation * simulation = new game::Simulation(g_hWnd);
	SetTimer(g_hWnd, (UINT_PTR)(simulation), 16, (TIMERPROC)OnTimer); //aim for 60 FPS

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPPBLOCKS));
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	KillTimer(g_hWnd, (UINT_PTR)(simulation));
	delete simulation;

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPBLOCKS));
	wcex.hCursor        = NULL;
	wcex.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // Store instance handle in our global variable

	const POINT ptZero = { 0, 0 };
	HMONITOR primryMonitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO primaryMonitorInfo = { sizeof(primaryMonitorInfo) };
	if (GetMonitorInfo(primryMonitor, &primaryMonitorInfo)) {
		g_hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP, 0, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

		if (!g_hWnd) {
			return FALSE;
		}

		ShowWindow(g_hWnd, nCmdShow);
		UpdateWindow(g_hWnd);
		::SendMessage(g_hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	} else {
		return FALSE;
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			switch (wmId) {
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_NOTABOUT:
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
		case WM_SETCURSOR: {
			if (LOWORD(lParam) == HTCLIENT) {
				SetCursor(NULL);
				return TRUE;
			}
		} break;
		case WM_LBUTTONUP:
			mouse.clicked = true;
			break;
		case WM_MOUSEMOVE:
			mouse.x = LOWORD(lParam);
			mouse.y = HIWORD(lParam);
			break;
		case WM_KEYUP:
			if (wParam == VK_ESCAPE) {
				PostQuitMessage(0);
			}
		break;
		case WM_GETMINMAXINFO: {
			LPMINMAXINFO lpmmi = reinterpret_cast<LPMINMAXINFO>(lParam);
			lpmmi->ptMinTrackSize.x = 320;
			lpmmi->ptMinTrackSize.y = 240;
		} return 0;
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		} break;
		case WM_ERASEBKGND:
			return 1;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void OnTimer(HWND hWnd, UINT uint, UINT_PTR ptr, DWORD word) {
	game::Simulation * simulation = reinterpret_cast<game::Simulation*>(ptr);
	if (g_rect.right == 0) {
		GetClientRect(hWnd, &g_rect);
	}
	if (g_hDC == INVALID_HANDLE_VALUE) {
		g_hDC = GetDC(hWnd);
	}
	simulation->UpdateAndDraw(&g_rect, g_hDC, mouse);
}
