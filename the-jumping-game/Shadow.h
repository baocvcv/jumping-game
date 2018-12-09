#pragma once
#include "the-jumping-game.h"

#define SHADOW_SRC_WIDTH 58
#define SHADOW_SRC_HEIGHT 100
#define SHADOW_WIDTH 38
#define SHADOW_HEIGHT 70
#define SHADOW_LAG 100

class Shadow {
public:
	Shadow();
	~Shadow();
	void setPos(int _x, int _y) { posX = _x; posY = _y; }
	int getX() { return posX; }
	int getY() { return posY; }

	void reset();
	void record(Coordinates _pos);
	void render(HDC bmp_buffer, HDC hdc_loadbmp, int cameraX, int cameraY);
	bool isChasing() { return inAction; }
	void canChase() { letsGo = true; }

private:
	HBITMAP img;
	int posX, posY;
	bool inAction;
	bool letsGo;
	bool facingRight;
	std::vector<Coordinates> history;
};

