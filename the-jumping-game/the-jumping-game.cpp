// the-jumping-game.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "the-jumping-game.h"
#include "Game.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


int MENU_INFO[NO_OF_MENUS][MAX_BUTTONS_PER_MENU + 1][3] = {
	{{MENU_START, 1}, {BUTTON_START_GAME, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}},
	{{MENU_HELP , 1}, {BUTTON_START_GAME, 0, 0}}
};

int MAP_TEST[9][16] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
						0,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,0,
						0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
						0,1,0,0, 0,0,0,0, 0,0,0,0, 0,1,0,0, // 4 
						0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
						0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
						0,0,0,0, 1,1,1,1, 0,0,0,0, 0,0,0,0,
						0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,1,0, // 8
						1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1 };
int MAP_TEST_WIDTH = 16;
int MAP_TEST_HEIGHT = 9;


std::map<int, int> bmp_resources;
std::map<int, int> texture_resources;
std::map<int, std::string> map_files;
std::map<int, int> button_action;
Game game;

int mouseX, mouseY;
std::map<int, bool> key_status;
bool mouseLeftButtonDown = false;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void InitGame(HWND hWnd, WPARAM wParam, LPARAM lParam);
void Paint(HWND hWnd); // draws everything
void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam); // update timer

void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
void MouseLeftDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
void MouseLeftUp(HWND hWnd, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_THEJUMPINGGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_THEJUMPINGGAME));

    MSG msg;

    // Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_THEJUMPINGGAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_THEJUMPINGGAME);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, 
	   WS_OVERLAPPEDWINDOW, 
	   CW_USEDEFAULT, 
	   CW_USEDEFAULT, 
	   WINDOW_WIDTH+10, // accounting for app borders
	   WINDOW_HEIGHT+WINDOW_TITLEBAR_HEIGHT,
	   nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		// initialize game
		InitGame(hWnd, wParam, lParam);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
	case WM_KEYDOWN:
		// handles key presses
		game.keyEvent(wParam, true);
		break;
	case WM_KEYUP:
		// handles key releases
		game.keyEvent(wParam, false);
		break;
	case WM_MOUSEMOVE:
		// handles mouse movements
		//MouseMove(hWnd, wParam, lParam);
		game.mouseEvent(LOWORD(lParam), HIWORD(lParam), false);
		break;
	case WM_LBUTTONDOWN:
		// handles left click
		//MouseLeftDown(hWnd, wParam, lParam);
		game.mouseEvent(LOWORD(lParam), HIWORD(lParam), true);
		break;
	case WM_LBUTTONUP:
		// handles left click
		//MouseLeftUp(hWnd, wParam, lParam);
		game.mouseEvent(LOWORD(lParam), HIWORD(lParam), false);
		break;
	case WM_TIMER:
		TimerUpdate(hWnd, wParam, lParam);
    case WM_PAINT:
        {
			Paint(hWnd);
			break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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

#pragma region Processes
void InitGame(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	bmp_resources[BUTTON_START_GAME] = IDB_BITMAP_BUTTON_START_GAME;
	bmp_resources[MENU_START] = IDB_BITMAP_MENU_START;
	bmp_resources[MENU_HELP] = IDB_BITMAP_MENU_START;
	bmp_resources[STAGE_0] = IDB_BITMAP_STAGE0;

	texture_resources[STAGE_0] = IDB_BITMAP_TEXTURE0;

	map_files[STAGE_0] = "../resources/stage0.data";

	button_action[BUTTON_START_GAME] = STAGE_0;

	key_status[VK_UP] = false;
	key_status[VK_DOWN] = false;
	key_status[VK_LEFT] = false;
	key_status[VK_RIGHT] = false;
	key_status[VK_LEFT] = false;

	game.init();

	SetTimer(hWnd, TIMER_GAMETIMER, TIMER_GAMETIMER_ELAPSE, NULL);
}

void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	game.update();

	InvalidateRect(hWnd, NULL, false);
}

void Paint(HWND hWnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HDC hdc_membuffer = CreateCompatibleDC(hdc);
	HDC hdc_loadbmp = CreateCompatibleDC(hdc);

	// initialize hdc _membuffer
	HBITMAP blankBmp = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdc_membuffer, blankBmp);
	/* test
	HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_BUTTON_START_GAME));
	Button* button = new Button(BUTTON_START_GAME, bitmap);
	button->isVisible = true;
	button->posX = 0;
	button->posY = 0;
	button->width = BUTTON_BMP_WIDTH;
	button->height = BUTTON_BMP_HEIGHT;

	HDC hdc_membuffer = CreateCompatibleDC(hdc);
	HDC hdc_loadbmp = CreateCompatibleDC(hdc);
	HBITMAP blankBmp = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdc_membuffer, blankBmp);

	button->render(hdc, hdc_membuffer, hdc_loadbmp);
	SelectObject(hdc_loadbmp, button->bmp_Button);
	TransparentBlt(
		hdc_membuffer, 200, 0, BUTTON_BMP_WIDTH, BUTTON_BMP_HEIGHT,
		hdc_loadbmp, 0, 0, BUTTON_BMP_WIDTH, BUTTON_BMP_HEIGHT,
		RGB(255, 255, 255)
	);
	BitBlt(
		hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		hdc_membuffer, 0, 0, SRCCOPY
	);*/
	game.render(hdc_membuffer, hdc_loadbmp);

	BitBlt(
		hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		hdc_membuffer, 0, 0, SRCCOPY
	);

	DeleteObject(blankBmp);
	DeleteDC(hdc_membuffer);
	DeleteDC(hdc_loadbmp);

	EndPaint(hWnd, &ps);
}
#pragma endregion

#pragma region EventHandling
void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	game.mouseEvent(LOWORD(lParam), HIWORD(lParam), false);
}

void MouseLeftDown(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	game.mouseEvent(LOWORD(lParam), HIWORD(lParam), true);
}

void MouseLeftUp(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	game.mouseEvent(LOWORD(lParam), HIWORD(lParam), false);
}
#pragma endregion
