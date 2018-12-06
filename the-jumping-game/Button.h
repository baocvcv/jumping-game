#pragma once

#include "the-jumping-game.h"

#define BUTTON_WIDTH 285
#define BUTTON_HEIGHT 53
#define FRAME_CLICK 1
#define FRAME_FOCUS 2

class Button
{
public:
	// methods
	Button(int _id, HBITMAP _bmp_Button);
	~Button();
	bool isHovered(int mouseX, int mouseY);
	void render(HDC bmp_buffer, HDC hdc_loadbmp);

	void setClicked(bool _isClicked) { isClicked = _isClicked; }
	void setPos(int x, int y) { posX = x; posY = y; }
	void setVisible(bool visible) { isVisible = visible; }
	int id() { return buttonId; }
	
private:
	int buttonId;
	HBITMAP bmp_Button;
	int posX, posY, width, height;
	bool isVisible;
	bool isHoveredOn;
	bool isClicked;
};

