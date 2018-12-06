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
	isAnimated = false;
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

	int x = posX;
	int y = posY;
	int wid = width;
	int hei = height;
	if (isAnimated) {
		if (isReversed) {
			x = posX - (posX - fromX) * frame_count / totalFrames;
			y = posY - (posY - fromY) * frame_count / totalFrames;
			double r = 1.0 - (1.0 - ratioStart) * frame_count / totalFrames;
			wid = width * r;
			hei = height * r;
		}
		else {
			x = fromX + (posX - fromX) * frame_count / totalFrames;
			y = fromY + (posY - fromY) * frame_count / totalFrames;
			double r = ratioStart + (1.0 - ratioStart) * frame_count / totalFrames;
			wid = width * r;
			hei = height * r;
		}
		frame_count++;
		if (frame_count > totalFrames) {
			isAnimated = false;
		}
	}
	TransparentBlt(
		bmp_buffer, x, y, wid, hei,
		hdc_loadbmp, 0, BUTTON_HEIGHT*frame, BUTTON_WIDTH, BUTTON_HEIGHT,
		RGB(255, 255, 255)
	);
}

void Button::animate(int _fromX, int _fromY, int _totalFrames, double _ratio, bool _reverse) {
	isAnimated = true;
	frame_count = 0;
	fromX = _fromX;
	fromY = _fromY;
	totalFrames = _totalFrames;
	ratioStart = _ratio;
	isReversed = _reverse;
}

Button::~Button()
{
}
