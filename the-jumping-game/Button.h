#pragma once

#include "the-jumping-game.h"

#define BUTTON_WIDTH 285
#define BUTTON_HEIGHT 53
//#define BUTTON_WIDTH 300
//#define BUTTON_HEIGHT 100
#define FRAME_CLICK 1
#define FRAME_FOCUS 2

class Button
{
public:
	// methods
	Button(int _id, HBITMAP _bmp_Button);
	~Button();
	void setClicked(bool _isClicked) { isClicked = _isClicked; }
	void setPos(int x, int y) { posX = x; posY = y; }
	void setVisible(bool visible) { isVisible = visible; }

	int id() { return buttonId; }
	int getX() { return posX; }
	int getY() { return posY; }
	bool isAnimating() { return isAnimated; }
	bool isHovered(int mouseX, int mouseY);

	void render(HDC bmp_buffer, HDC hdc_loadbmp);
	void animate(int fromX, int fromY, int totalFrames, double ratio, bool direction);

private:
	int buttonId;
	HBITMAP bmp_Button;
	int posX, posY, width, height;
	bool isVisible;
	bool isHoveredOn;
	bool isClicked;
	
	// animations
	bool isAnimated;
	bool isReversed; // animation direction
	int fromX, fromY;
	int frame_count;
	int totalFrames;
	double ratioStart;
};

