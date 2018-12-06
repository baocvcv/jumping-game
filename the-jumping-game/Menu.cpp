#include "stdafx.h"
#include "Menu.h"


Menu::Menu()
{
}

Menu::Menu(HBITMAP _bmp_Background, int menuInfo[MAX_BUTTONS_PER_MENU][3]) {
	bmp_Background = _bmp_Background;
	
	menuId = menuInfo[0][0];
	int no_of_buttons = menuInfo[0][1];
	for (int i = 1; i <= no_of_buttons; i++) {
		int bmpId = bmp_resources[menuInfo[i][0]];
		HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(bmpId));
		Button button(menuInfo[i][0], bitmap);
		button.setPos(menuInfo[i][1], menuInfo[i][2]);
		button.setVisible(true);
		buttons.push_back(button);
	}

	animation_type = 0; // no animation
}

void Menu::setParentMap(int _mapId) {
	parentMap = _mapId;
	button_action[BUTTON_RESUME_GAME] = parentMap + 1 * 1000;
	button_action[BUTTON_RESTART_STAGE] = parentMap + 2 * 1000;
}

void Menu::render(HDC bmp_buffer, HDC hdc_loadbmp) {
	SelectObject(hdc_loadbmp, bmp_Background);
	BitBlt(bmp_buffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,hdc_loadbmp, 0, 0, SRCCOPY);

	for (unsigned int i = 0; i < buttons.size(); i++) {
		buttons[i].render(bmp_buffer, hdc_loadbmp);
	}
}

void Menu::animate(int whatAnimation) {
	animation_type = whatAnimation;
	double ratio = 0.3;
	switch (animation_type) {
	case 1:
		for (unsigned int i = 0; i < buttons.size(); i++) {
			int x = buttons[i].getX();
			int y = buttons[i].getY();
			//buttons[i].animate(x, y, MENU_ANIMATION_LEN, 0.5);  // effect 1
			buttons[i].animate(x+(1-ratio)*BUTTON_WIDTH/2, y+100, BUTTON_ANIMATION_LEN+i*5, ratio, false); // effect 2
		}
		break;
	case 2:
		for (unsigned int i = 0; i < buttons.size(); i++) {
			int x = buttons[i].getX();
			int y = buttons[i].getY();
			buttons[i].animate(x + (1 - ratio)*BUTTON_WIDTH / 2, y + 100, BUTTON_ANIMATION_LEN, ratio, true); // effect 2
		}
		break;
	}
}

void Menu::eventMouseMove(int x, int y) {
	for (unsigned int i = 0; i < buttons.size(); i++) {
		buttons[i].isHovered(x, y);
	}
}

int Menu::eventMouseLeftDown(int x, int y) {
	for (unsigned int i = 0; i < buttons.size(); i++) {
		if (buttons[i].isHovered(x, y)) {
			buttons[i].setClicked(true);
			return button_action[buttons[i].id()];
		}
	}
	return -1;
}

int Menu::eventMouseLeftUp(int x, int y) {
	for (unsigned int i = 0; i < buttons.size(); i++) {
		buttons[i].setClicked(false);
		if (buttons[i].isHovered(x, y)) {
			return button_action[buttons[i].id()];
		}
	}
	return -1;
}

int Menu::id() {
	return menuId;
}

Menu::~Menu()
{
}
