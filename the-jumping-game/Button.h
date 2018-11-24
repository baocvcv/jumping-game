#pragma once

#include "the-jumping-game.h"

#define BUTTON_WIDTH 147
#define BUTTON_HEIGHT 60

class Button
{
public:
	// methods
	Button(int _id, HBITMAP _bmp_Button);
	~Button();
	bool isHovered(int mouseX, int mouseY);
	void render(HDC bmp_buffer, HDC hdc_loadbmp);

	void setPos(int x, int y) { posX = x; posY = y; }
	void setVisible(bool visible) { isVisible = visible; }
	int id() { return buttonId; }
	
private:
	int buttonId;
	HBITMAP bmp_Button;
	int posX, posY, width, height;
	bool isVisible;
	bool isHoveredOn;	
};

