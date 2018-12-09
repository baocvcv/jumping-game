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
	{{MENU_START, 2}, {BUTTON_START_GAME, 100, 100},{BUTTON_START_TUTORIAL, 100, 150 + BUTTON_HEIGHT}},
	{{MENU_HELP , 1}, {BUTTON_BACK, 100, 100}},
	{{MENU_PAUSE , 3}, {BUTTON_RESUME_GAME, 100, 100},{BUTTON_RESTART_STAGE, 100, 150 + BUTTON_HEIGHT},{BUTTON_BACK, 100, 100 + 2*(50+BUTTON_HEIGHT)}},
	{{MENU_SELECT_STAGE, 3}, {BUTTON_BACK, 100, 100}, {BUTTON_STAGE0, 100+ (50+BUTTON_WIDTH), 100}, {BUTTON_STAGE1, 100 + (50 + BUTTON_WIDTH), 100+(50+BUTTON_HEIGHT)}}
};

std::map<int, int> bmp_resources;
std::map<int, int> texture_resources;
std::map<int, std::string> map_files;
/* last 4 digits are scene id, rest are actions
0-switch scene 1-resume game 2-restart map
*/
std::map<int, int> button_action; 

HDC hdc;
Game game;
HWND hWnd_main;

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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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

   hWnd_main = hWnd;
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
		game.keyEvent(wParam, true);
		break;
	case WM_KEYUP:
		game.keyEvent(wParam, false);
		break;
	case WM_MOUSEMOVE:
		game.mouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:
		game.mouseEvent(LOWORD(lParam), HIWORD(lParam), true);
		break;
	case WM_LBUTTONUP:
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
	case MM_MCINOTIFY:
		if (wParam == MCI_NOTIFY_SUCCESSFUL) {
			if (lParam == 1) PlayWav("BGM2", "0", "", hWnd);
			else PlayWav("BGM1", "0", "", hWnd);
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
	// button bmp
	bmp_resources[BUTTON_START_GAME] = IDB_BITMAP_BUTTON_START_GAME;
	bmp_resources[BUTTON_START_TUTORIAL] = IDB_BITMAP_BUTTON_TUTORIAL;
	bmp_resources[BUTTON_RESUME_GAME] = IDB_BITMAP_BUTTON_CONTINUE;
	bmp_resources[BUTTON_RESTART_STAGE] = IDB_BITMAP_BUTTON_RESTART;
	bmp_resources[BUTTON_BACK] = IDB_BITMAP_BUTTON_BACK;
	bmp_resources[BUTTON_STAGE0] = IDB_BITMAP_BUTTON_STAGE0;
	bmp_resources[BUTTON_STAGE1] = IDB_BITMAP_BUTTON_STAGE1;
	
	// menu bkg
	bmp_resources[MENU_START] = IDB_BITMAP_MENU_START;
	bmp_resources[MENU_HELP] = IDB_BITMAP_MENU_START;
	bmp_resources[MENU_PAUSE] = IDB_BITMAP_MENU_START;
	bmp_resources[MENU_SELECT_STAGE] = IDB_BITMAP_MENU_START;

	// map bkg
	bmp_resources[STAGE_0] = IDB_BITMAP_STAGE0;
	bmp_resources[STAGE_1] = IDB_BITMAP_STAGE0;

	// texture resource
	texture_resources[STAGE_0] = IDB_BITMAP_TEXTURE0;
	texture_resources[STAGE_1] = IDB_BITMAP_TEXTURE0;

	// map files
	map_files[STAGE_0] = "../resources/stage0.data";
	map_files[STAGE_1] = "../resources/stage1.data";

	// button action
	button_action[BUTTON_START_GAME] = MENU_SELECT_STAGE;
	button_action[BUTTON_START_TUTORIAL] = MENU_HELP;
	button_action[BUTTON_BACK] = MENU_START;

	button_action[BUTTON_STAGE0] = STAGE_0;
	button_action[BUTTON_STAGE1] = STAGE_1;

	// key status
	key_status[VK_UP] = false;
	key_status[VK_DOWN] = false;
	key_status[VK_LEFT] = false;
	key_status[VK_RIGHT] = false;
	key_status[VK_LEFT] = false;

	// load music resources
	LoadWav("bgm1.wav", "BGM1");
	LoadWav("bgm2.wav", "BGM2");
	LoadWav("born.wav", "BORN");
	LoadWav("button_press.wav", "BUTTON");
	LoadWav("jump.wav", "JUMP");
	LoadWav("charge.wav", "CHARGE");
	LoadWav("die.wav", "DIE");
	LoadWav("succeed.wav", "SUCCEED");

	PlayWav("BGM1", "0", "", hWnd);

	game.init();
	SetTimer(hWnd, TIMER_GAMETIMER, TIMER_GAMETIMER_ELAPSE, NULL);
}

void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	game.update();
	InvalidateRect(hWnd, NULL, false);
}

void Paint(HWND hWnd) {
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	HDC hdc_membuffer = CreateCompatibleDC(hdc);
	HDC hdc_loadbmp = CreateCompatibleDC(hdc);

	// initialize hdc _membuffer
	HBITMAP blankBmp = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdc_membuffer, blankBmp);

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

void LoadWav(std::string filename, std::string alias) {
	std::string prefix("open \"..\\resources\\sound\\");
	std::string cmd = prefix + filename + "\" " + "type waveaudio alias " + alias;
	mciSendStringA(cmd.c_str(), NULL, 0, NULL);
}

void PlayWav(std::string alias, std::string from, std::string to, HWND hWnd) {
	std::string prefix("play ");
	std::string cmd = prefix + alias + " from " + from;
	if (to.length() > 0)
		cmd += " to " + to;
	if (hWnd != NULL) {
		cmd += " notify";
		mciSendStringA(cmd.c_str(), NULL, 0, hWnd);
	}
	else {
		mciSendStringA(cmd.c_str(), NULL, 0, NULL);
	}
}
#pragma endregion
