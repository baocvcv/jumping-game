#pragma once

#include <Windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string.h>

#include "resource.h"
#pragma comment(lib, "Msimg32.lib")

#include <vector>
#include <string>
#include <map>
#include <math.h>


// Globals
#define WINDOW_WIDTH			1600
#define WINDOW_HEIGHT			900
#define WINDOW_TITLEBAR_HEIGHT  32

#define TIMER_GAMETIMER			1
#define TIMER_GAMETIMER_ELAPSE	10

#define NO_OF_MENUS				3
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

#define STAGE_0					100

#define BUTTON_START_GAME       200
#define BUTTON_START_TUTORIAL   201
#define BUTTON_RESUME_GAME		202
#define BUTTON_RESTART_STAGE	203
#define BUTTON_BACK				204


typedef std::pair<int, int> Coordinates;
typedef std::pair<double, double> Speed;

extern HINSTANCE hInst;
extern HDC hdc;
// input status
extern int mouseX, mouseY;
extern bool mouseLeftButtonDown;
extern std::map<int, bool> key_status; // if a key is pressed


extern std::map<int, int> bmp_resources; // maps id to bmp resource
extern std::map<int, int> texture_resources; // maps stage id to texture resources
extern std::map<int, std::string> map_files; // map filenames
extern std::map<int, int> button_action; // maps id to action
/* 
menu info:
	menu id, num of buttons; each line is a button
*/
extern int MENU_INFO[NO_OF_MENUS][MAX_BUTTONS_PER_MENU + 1][3];

/*
tile info:

*/