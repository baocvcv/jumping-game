#pragma once

#include <Windows.h>
#include <mmsystem.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string.h>
#include <math.h>

#include "resource.h"
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "Winmm.lib")

#include <vector>
#include <string>
#include <map>
#include <stack>

#define WINDOW_WIDTH			1600
#define WINDOW_HEIGHT			900
#define WINDOW_TITLEBAR_HEIGHT  50

#define TIMER_GAMETIMER			1
#define TIMER_GAMETIMER_ELAPSE	10

#define NO_OF_MENUS				4
#define NO_OF_STAGES			1
#define MAX_BUTTONS_PER_MENU	10

/* 
resource id:
	0-99    menu
	100-199 stage
	200-299 button
*/
#define MENU_START				0
#define MENU_HELP				1
#define MENU_PAUSE				2
#define MENU_SELECT_STAGE		3

#define STAGE_HELP				100
#define STAGE_0					101
#define STAGE_1					102
#define	STAGE_2					103
#define STAGE_10				111
#define STAGE_11				112
#define STAGE_12				113

#define BUTTON_START_GAME       200
#define BUTTON_TO_TUTORIAL		201
#define BUTTON_RESUME_GAME		202
#define BUTTON_RESTART_STAGE	203
#define BUTTON_BACK				204

#define BUTTON_STAGEHELP		210
#define BUTTON_STAGE0			211
#define BUTTON_STAGE1			212
#define	BUTTON_STAGE2			213
#define BUTTON_STAGE10			221
#define BUTTON_STAGE11			222
#define BUTTON_STAGE12			223


typedef std::pair<int, int> Coordinates;
typedef std::pair<double, double> Speed;

/*
globals
*/
extern HINSTANCE hInst;
extern HDC hdc;

extern int mouseX, mouseY;
extern bool mouseLeftButtonDown;
extern std::map<int, bool> key_status;		 // if a key is pressed

extern std::map<int, int> bmp_resources;     // maps id to bmp resource
extern std::map<int, int> texture_resources; // maps stage id to texture resources
extern std::map<int, std::string> map_files; // map filenames
extern std::map<int, int> button_action;     // maps id to action

/*
functions
*/

// loadwav from pre-defined path, give name alias
void LoadWav(std::string filename, std::string alias);

// playwav: if hWnd is given, wav will be repeatedly played
void PlayWav(std::string alias, std::string from, std::string to, HWND hWnd);