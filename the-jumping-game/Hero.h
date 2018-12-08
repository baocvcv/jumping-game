#pragma once

#include "the-jumping-game.h"

#define HERO_SPEED_X	6.0
#define HERO_SPEED_Y	16.0
#define HERO_JUMP_SPEED 16.0
#define HERO_GRAVITY	1.0
#define HERO_ACCELX		3.0
#define HERO_CHARGE_SPEED 18.0
#define HERO_CHARGE_ACCEL 0.5

#define HERO_BMP_WIDTH  58
#define HERO_BMP_HEIGHT 100
#define HERO_WIDTH		38
#define HERO_HEIGHT		70
#define BORDER_POINT_NUM 20

#define HERO_NUM_ABILITIES 7

extern int Nodes_Hero[BORDER_POINT_NUM][2];
extern int Borders_Hero[4][BORDER_POINT_NUM / 4 + 1];

//TODO: animation- climb
//TODO: ability- climbing
//TODO: sound effects

enum Talents {
	STAND,
	WALK,
	JUMP,
	CHARGE,
	CLIMB,
	BORN,
	DIE,
}; 

class Ability {
public:
	Ability() {}
	Ability(Talents _id, int _nF, bool _us, bool _isU, int fC, int as) : 
		id(_id), nFrame(_nF), usable(_us), isUsing(_isU), frameCount(fC), animation_speed(as) {}

	Talents id; // id*2 + facingright = frame.x
	int nFrame;
	bool usable;
	bool isUsing;
	int frameCount;
	int animation_speed;
};

class Hero
{
public:
	Hero();
	Hero(int _posX, int _posY, HBITMAP _img);

	// change status
	void reset(int x, int y);
	void setPos(int x, int y) { posX = x; posY = y; }
	void setSpeed(int vx, int vy) { speedX = vx * HERO_SPEED_X; speedY = vy * HERO_SPEED_Y;}
	void setSpeedX(int vx) { speedX = vx * HERO_SPEED_X; }
	void setSpeedY(int vy) { speedY = vy * HERO_SPEED_Y; }
	void die(int how);

	void setOnGround(bool isOn); // set status to if on the ground
	void hitVerticalWall(int _whatWall); // hits vertical wall
	void setOnEdge(bool isOn) { onEdge = isOn; }

	// get status
	Coordinates getPos() { return std::make_pair(posX, posY); }
	Speed getSpeed() { return std::make_pair(speedX, speedY); }
	std::vector<Coordinates> getBorderNodes(int _border); // 0up 1right 2down 3left 4all
	Talents getStatus() { return status; }

	// move
	void regularJump() { speedY = -HERO_JUMP_SPEED; inAir = true; } //HACK:: change back
	void charge(Speed _dir); // charge
	void useAbility(int _id, int p1, int p2);

	// animation
	void setImg(HBITMAP _img) { img = _img; }

	// events
	void keyEvent(int _key, bool pressed);
	void update();
	void render(HDC bmp_buffer, HDC hdc_loadbmp, int cameraX, int cameraY);

	~Hero();

private:
	HBITMAP img;
	long posX, posY;
	int width, height;

	bool inAir;
	int whatWall;
	bool onEdge;

	bool facingRight;

	std::vector<Ability> Abilities;
	Talents status;

	Speed chargeDir;

	double speedX, speedY;
};

