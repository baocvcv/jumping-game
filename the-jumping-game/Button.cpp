#include "stdafx.h"
#include "Button.h"


Button::Button(int _id , HBITMAP _bmp_Button)
{
	buttonId = _id;
	bmp_Button = _bmp_Button;
	posX = 0;
	posY = 0;
	width = BUTTON_WIDTH;
	height = BUTTON_HEIGHT;
	isVisible = false;
	isHoveredOn = false;
}

bool Button::isHovered(int mouseX, int mouseY) {
	if (mouseX >= posX && 
		mouseX <= posX + width && 
		mouseY >= posY && 
		mouseY <= posY + height) {
		isHoveredOn = true;
		return true;
	}
	else {
		isHoveredOn = false;
		return false;
	}
}

void Button::render(HDC bmp_buffer, HDC hdc_loadbmp) {
	if (!isVisible) {
		return;
	}
	int frame = 0;
	if (isClicked && isHoveredOn)
		frame = FRAME_CLICK;
	else if (isHoveredOn)
		frame = FRAME_FOCUS;

	SelectObject(hdc_loadbmp, bmp_Button);
	TransparentBlt(
		bmp_buffer, posX, posY,
		width, height,
		hdc_loadbmp, 0, BUTTON_HEIGHT*frame, BUTTON_WIDTH, BUTTON_HEIGHT,
		RGB(255, 255, 255)
	);
}


Button::~Button()
{
}
