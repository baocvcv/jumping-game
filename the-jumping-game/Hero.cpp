#include "stdafx.h"
#include "Hero.h"

#include <cmath>

#define DIST(x,y) (pow(x*x+y*y, 0.5))

int Nodes_Hero[BORDER_POINT_NUM][2] = {
	{0,0},{HERO_WIDTH / 5, 0},{HERO_WIDTH * 2 / 5, 0},{HERO_WIDTH * 3 / 5, 0},{HERO_WIDTH * 4 / 5, 0},
	{HERO_WIDTH-1,0},{HERO_WIDTH - 1, HERO_HEIGHT / 5}, {HERO_WIDTH - 1, HERO_HEIGHT * 2 / 5},{HERO_WIDTH - 1, HERO_HEIGHT * 3 / 5},{HERO_WIDTH - 1, HERO_HEIGHT * 4 / 5},
	{HERO_WIDTH - 1,HERO_HEIGHT - 1},{HERO_WIDTH * 4 / 5, HERO_HEIGHT - 1} ,{HERO_WIDTH * 3 / 5, HERO_HEIGHT - 1} ,{HERO_WIDTH *2 / 5, HERO_HEIGHT - 1} ,{HERO_WIDTH / 5, HERO_HEIGHT - 1},
	{0,HERO_HEIGHT-1},{0,HERO_HEIGHT * 4 / 5} ,{0,HERO_HEIGHT * 3 / 5} ,{0,HERO_HEIGHT * 2 / 5} ,{0,HERO_HEIGHT / 5}
};
int Borders_Hero[4][BORDER_POINT_NUM / 4+1] = { {0,1,2,3,4,5}, {5,6,7,8,9,10},{10,11,12,13,14,15}, {15,16,17,18,19,0} };

Hero::Hero()
{
	img = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_HERO));

	width = HERO_WIDTH;
	height = HERO_HEIGHT;

	reset(-1, -1);
}

Hero::Hero(int _posX, int _posY, HBITMAP _img) {
	img = _img;
	reset(_posX, _posY);
}

 /* change status */
void Hero::reset(int x, int y) {
	setPos(x, y);
	setSpeed(0, 0);

	hasCharged = false;
	inAir = true;
	whatWall = 0;
	facingRight = true;
}

void Hero::onGround(bool isOn) {
	if (isOn) {
		speedY = 0.0;
		inAir = false;
		if(!isCharging)
			hasCharged = false;
	}
	else {
		inAir = true;
	}
}

void Hero::hitVerticalWall(int _whatWall) {
	whatWall = _whatWall; 
	if (whatWall != 0) {
		speedX = 0.0;
	}
}

/* get status */
std::vector<std::pair<int, int>> Hero::getBorderNodes(int _border) {
	std::vector<std::pair<int, int>> nodes;
	if (_border == 4) {
		for (int i = 0; i < BORDER_POINT_NUM; i++) {
				nodes.push_back(std::make_pair(posX+Nodes_Hero[i][0], posY+Nodes_Hero[i][1]));
			}
	}
	else {
		for (int i = 0; i < BORDER_POINT_NUM / 4 + 1; i++) {
			nodes.push_back(std::make_pair(posX + Nodes_Hero[Borders_Hero[_border][i]][0], posY + Nodes_Hero[Borders_Hero[_border][i]][1]));
		}
	}
	return nodes;
}

/* events */
void Hero::keyEvent(int _key, bool pressed) {
	if (pressed) {
		switch (_key) {
		case 'C':
			if(!inAir)
				regularJump();
			break;
		case 'X':
			if (!hasCharged) {
				Speed dir;
				dir.first = 0.0; dir.second = 0.0;
				if (key_status[VK_LEFT]) dir.first -= 1.0;
				if (key_status[VK_RIGHT]) dir.first += 1.0;
				if (key_status[VK_UP]) dir.second -= 1.0;
				if (key_status[VK_DOWN]) dir.second += 1.0;
				if (!key_status[VK_LEFT] && !key_status[VK_RIGHT] && !key_status[VK_UP] && !key_status[VK_DOWN]) {
					dir.first = 1.0;
					dir.second = 0.0;
				}
				if (abs(dir.second) < 0.1 && abs(dir.first) < 0.1) break;
				charge(dir);
			}
		}
	}
}

void Hero::charge(Speed _dir) {
	chargeDir = _dir;
	hasCharged = true;
	isCharging = true;
	chargeFrame = 0;
	speedX = HERO_CHARGE_SPEED * chargeDir.first;
	speedY = HERO_CHARGE_SPEED * chargeDir.second;
	if (chargeDir.second < 0.0)
		inAir = true;
}
void Hero::die(int how) {

}


void Hero::update() {
	// change direction faced
	if (speedX > 0.0) facingRight = true;
	else if (speedX < 0.0) facingRight = false;

	if (isCharging) {
		// charge
		speedX -= HERO_CHARGE_ACCEL * chargeDir.first;
		speedY -= HERO_CHARGE_ACCEL * chargeDir.second;
		if (speedX * chargeDir.first < 0) speedX = 0;
		if (speedY * chargeDir.second < 0) speedY = 0;
		chargeFrame++;
		if (chargeFrame > HERO_CHARGE_FRAME) {
			stopCharge();
		}
	}

	// speed Y
	if (inAir) {
		if (!isCharging) {
			speedY += HERO_GRAVITY;
			if (speedY >= HERO_SPEED_Y) speedY = HERO_SPEED_Y;
		}
	}
	else speedY = 0;

	if (!isCharging) {
		// speed X
		if (key_status[VK_LEFT] && !key_status[VK_RIGHT] && speedX > -HERO_SPEED_X) {
			speedX -= HERO_ACCELX; // press left
		}
		else if (!key_status[VK_LEFT] && key_status[VK_RIGHT] && speedX < HERO_SPEED_X) {
			speedX += HERO_ACCELX; // press right key
		}
		else if (speedX > 0) {
			speedX -= HERO_ACCELX;
		}
		else if (speedX < 0) {
			speedX += HERO_ACCELX;
		}
	}

	// hit wall
	if (whatWall == 1 && speedX > 0) {
		speedX = 0;
	}
	else if (whatWall == -1 && speedX < 0) {
		speedX = 0;
	}

	// position
	setPos(posX+(int)speedX, posY+(int)speedY);
}

void Hero::render(HDC bmp_buffer, HDC hdc_loadbmp, int cameraX, int cameraY) {
	SelectObject(hdc_loadbmp, img);
	TransparentBlt(
		bmp_buffer, posX-cameraX, posY-cameraY,
		width, height,
		hdc_loadbmp, 0, 0, HERO_BMP_WIDTH, HERO_BMP_HEIGHT,
		RGB(255, 255, 255)
	);
}

Hero::~Hero()
{
	//DeleteObject(img);
}
