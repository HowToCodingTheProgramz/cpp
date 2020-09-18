#include "framework.h"
#include "CPPBlocks.h"

#define MAX_LOADSTRING 100

HWND g_hWnd = (HWND)(INVALID_HANDLE_VALUE);
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

	game::Simulation * mps = new game::Simulation(g_hWnd);
	SetTimer(g_hWnd, (UINT_PTR)(mps), 16, (TIMERPROC)OnTimer); //aim for 60 FPS

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPPBLOCKS));
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	KillTimer(g_hWnd, (UINT_PTR)(mps));
	delete mps;

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
	wcex.hCursor        = LoadCursor(nullptr, IDC_HAND);
	wcex.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_CPPBLOCKS);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // Store instance handle in our global variable
	g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 100, 100, 640, 480, nullptr, nullptr, hInstance, nullptr);

	if (!g_hWnd) {
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

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
		case WM_LBUTTONUP:
			mouse.clicked = true;
			break;
		case WM_MOUSEMOVE:
			mouse.x = LOWORD(lParam);
			mouse.y = HIWORD(lParam);
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
	game::Simulation * mps = reinterpret_cast<game::Simulation*>(ptr);
	RECT rect;
	GetClientRect(hWnd, &rect);

	HDC hdc = GetDC(hWnd);
	mps->UpdateAndDraw(&rect, hdc, mouse);
	ReleaseDC(hWnd, hdc);
}
