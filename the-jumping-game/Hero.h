#pragma once
#include "the-jumping-game.h"

/*
max jump height: 5 tiles
max jump width: 10 tiles
max dash height: 11 t
max dash width: 11 t
*/
#define HERO_SPEED_X	6.0
#define HERO_SPEED_Y	18.0
#define HERO_JUMP_SPEED 18.0
#define HERO_GRAVITY	1.0
#define HERO_ACCELX		1.0
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

//TODO: climbing?
//HACK: sound effects, more?

enum Talent {
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
	Ability(Talent _id, int _nF, bool _us, bool _isU, int fC, int as) : 
		id(_id), nFrame(_nF), usable(_us), isUsing(_isU), frameCount(fC), animation_speed(as) {}

	Talent id; // id*2 + facingright = frame.x
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

	void setPos(int x, int y) { posX = x; posY = y; }
	void setSpeed(int vx, int vy) { speedX = vx * HERO_SPEED_X; speedY = vy * HERO_SPEED_Y;}
	void setSpeedX(int vx) { speedX = vx * HERO_SPEED_X; }
	void setSpeedY(int vy) { speedY = vy * HERO_SPEED_Y; }

	void setOnGround(bool isOn); // set status to if on the ground
	void hitVerticalWall(int _whatWall); // hits vertical wall
	void setOnEdge(bool isOn) { onEdge = isOn; }

	void reset(int x, int y);
	void enableAbility(Talent _id) { Abilities[_id].usable = true;}
	void useAbility(int _id, int param1, int param2); // use abilities

	// get status
	Coordinates getPos() { return std::make_pair(posX, posY); }
	Speed getSpeed() { return std::make_pair(speedX, speedY); }
	Talent getStatus() { return status; }
	bool hasMoved() { return inAction; }
	std::vector<Coordinates> getBorderNodes(int _border); // 0up 1right 2down 3left 4all

	// events
	void keyEvent(int _key, bool pressed);
	void update();
	void render(HDC bmp_buffer, HDC hdc_loadbmp, int cameraX, int cameraY);

	~Hero();

private:
	HBITMAP img;
	long posX, posY;
	int width, height;
	double speedX, speedY;
	
	Talent status;
	std::vector<Ability> Abilities;
	Speed chargeDir;

	bool inAction;
	bool inAir;
	int whatWall;
	bool onEdge;
	bool facingRight;
};

