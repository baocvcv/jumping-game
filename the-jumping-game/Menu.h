#pragma once

#include "the-jumping-game.h"
#include "Button.h"

#include <vector>

#define BUTTON_ANIMATION_LEN 10

using namespace std;

class Menu
{
public:
	Menu();
	Menu(HBITMAP _bmp_Background, int menuInfo[MAX_BUTTONS_PER_MENU][3]);

	void setParentMap(int _mapId);

	// events
	void eventMouseMove(int x, int y);
	int eventMouseLeftDown(int x, int y);
	int eventMouseLeftUp(int x, int y);

	void render(HDC bmp_buffer, HDC hdc_loadbmp);
	void animate(int whatAnimation); // set animation type
	bool isAnimationFinished() { return !(buttons.back().isAnimating()); }
	
	int id();

	~Menu();

private:
	int menuId;
	vector<Button> buttons;
	HBITMAP bmp_Background;
	// for pausing the game
	int parentMap;

	// for animation
	int animation_type; // 0-nothing 1-appear 2-disappear
	int frame_count;
};

