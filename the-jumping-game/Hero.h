#pragma once

#include "the-jumping-game.h"

#define HERO_SPEED_X	6.0
#define HERO_SPEED_Y	20.0
#define HERO_JUMP_SPEED 20.0
#define HERO_GRAVITY	2.0
#define HERO_ACCELX		1.0

#define HERO_BMP_WIDTH  115
#define HERO_BMP_HEIGHT 200
#define HERO_WIDTH		35
#define HERO_HEIGHT		60
#define BORDER_POINT_NUM 12

extern int Nodes_Hero[BORDER_POINT_NUM][2];
extern int Borders_Hero[4][BORDER_POINT_NUM / 4 + 1];

class Hero
{
public:
	Hero();
	Hero(int _posX, int _posY, HBITMAP _img);

	// change status
	void reset(int x, int y);
	void setPos(int x, int y) { posX = x; posY = y; }
	void setSpeed(int vx, int vy) { speedX = vx * HERO_SPEED_X; speedY = vy * HERO_SPEED_Y; }
	void setSpeedX(int vx) { speedX = vx * HERO_SPEED_X; }
	void setSpeedY(int vy) { speedY = vy * HERO_SPEED_Y; }

	void onGround(bool isOn); // set status to if on the ground
	void hitVerticalWall(int _whatWall); // hits vertical wall

	// get status
	Coordinates getPos() { return std::make_pair(posX, posY); }
	Speed getSpeed() { return std::make_pair(speedX, speedY); }
	std::vector<Coordinates> getBorderNodes(int _border); // 0up 1right 2down 3left 4all

	// move
	void regularJump() { speedY = -HERO_JUMP_SPEED; inAir = true; } //HACK:: change back

	// animation
	void setImg(HBITMAP _img) { img = _img; }

	// events
	void keyEvent();
	void update();
	void render(HDC bmp_buffer, HDC hdc_loadbmp);

	~Hero();

private:
	HBITMAP img;
	int posX, posY;
	int width, height;

	bool inAir;
	int whatWall;
	double speedX, speedY;
};

