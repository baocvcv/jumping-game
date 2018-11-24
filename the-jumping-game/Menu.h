#pragma once

#include "the-jumping-game.h"
#include "Button.h"

#include <vector>

using namespace std;

class Menu
{
public:
	Menu();
	Menu(HBITMAP _bmp_Background, int menuInfo[MAX_BUTTONS_PER_MENU][3]);

	// events
	void eventMouseMove(int x, int y);
	int eventMouseLeftClick(int x, int y);

	void render(HDC bmp_buffer, HDC hdc_loadbmp);
	
	int id();

	~Menu();

private:
	int menuId;
	vector<Button> buttons;
	HBITMAP bmp_Background;
};

