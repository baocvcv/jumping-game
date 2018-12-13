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
	void canChase() { letsGo = true; }

	int getX() { return posX; }
	int getY() { return posY; }
	bool isChasing() { return inAction; }

	void reset();
	void record(Coordinates _pos);
	void render(HDC bmp_buffer, HDC hdc_loadbmp, int cameraX, int cameraY);

private:
	HBITMAP img;
	int posX, posY;
	std::vector<Coordinates> history;

	bool inAction;
	bool letsGo;
	bool facingRight;
};

