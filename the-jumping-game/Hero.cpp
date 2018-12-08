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

int Hero_Abilities[HERO_NUM_ABILITIES][6] = {
	//id nFrame usable using frameCount
	{STAND, 1, 1, 0, 0, 1},
	{WALK, 10, 1, 0, 0, 4},
	{JUMP, 10, 0, 0, 0, 3},
	{CHARGE, 10, 1, 0, 0, 2},
	{CLIMB, 4, 0, 0, 0, 4},
	{BORN, 7, 1, 0, 0, 4},
	{DIE, 7, 1, 0, 0, 4},
};

Hero::Hero()
{
	img = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_HERO));

	width = HERO_WIDTH;
	height = HERO_HEIGHT;

	reset(-1, -1);

	/*
	Ability stand(STAND, 1, true, false, 0);
	abilities.push_back(stand);
	Ability walk(WALK, 9, true, false, 0);
	abilities.push_back(walk);*/
}

Hero::Hero(int _posX, int _posY, HBITMAP _img) {
	img = _img;
	reset(_posX, _posY);
}

 /* change status */
void Hero::reset(int x, int y) {
	setPos(x, y);
	setSpeed(0, 0);

	inAir = true;
	whatWall = 0;
	facingRight = true;
	onEdge = false;

	for (int i = 0; i < HERO_NUM_ABILITIES; i++) {
		Ability ability(
			Talents(Hero_Abilities[i][0]),
			Hero_Abilities[i][1],
			Hero_Abilities[i][2] == 1,
			Hero_Abilities[i][3] == 1,
			Hero_Abilities[i][4],
			Hero_Abilities[i][5]);
		Abilities.push_back(ability);
	}
	useAbility(BORN,0,0);
}

void Hero::setOnGround(bool isOn) {
	if (isOn) {
		speedY = 0.0;
		inAir = false;
		Abilities[JUMP].isUsing = false;
		Abilities[JUMP].usable = true;
		if (!Abilities[CHARGE].isUsing)
			Abilities[CHARGE].usable = true;
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
			if (!inAir)
				useAbility(JUMP, 0, 0);
				//regularJump();
			break;
		case 'X':
			if (Abilities[CHARGE].usable) {
				Coordinates dir;
				if (key_status[VK_LEFT]) dir.first -= 1;
				if (key_status[VK_RIGHT]) dir.first += 1;
				if (key_status[VK_UP]) dir.second -= 1;
				if (key_status[VK_DOWN]) dir.second += 1;
				if (!key_status[VK_LEFT] && !key_status[VK_RIGHT] && !key_status[VK_UP] && !key_status[VK_DOWN]) {
					dir.first = facingRight ? 1 : -1;
					dir.second = 0;
				}
				if (dir.second==0 && dir.first==0) break;
				useAbility(CHARGE, dir.first, dir.second);
			}
		}
	}
}

void Hero::useAbility(int _id, int p1, int p2) {
	switch (Talents(_id)) {
	case STAND:
		break;
	case WALK:
		break;
	case JUMP:
		status = JUMP;
		speedY = -HERO_JUMP_SPEED;
		inAir = true;
		Abilities[JUMP].usable = false;
		Abilities[JUMP].isUsing = true;
		Abilities[JUMP].frameCount = 0;
		break;
	case CHARGE:
		status = CHARGE;
		chargeDir.first = p1 / DIST(p1, p2);
		chargeDir.second = p2 / DIST(p1, p2);
		Abilities[CHARGE].usable = false;
		Abilities[CHARGE].isUsing = true;
		Abilities[CHARGE].frameCount = 0;
		speedX = HERO_CHARGE_SPEED * chargeDir.first;
		speedY = HERO_CHARGE_SPEED * chargeDir.second;
		if (chargeDir.second < 0.0)
			inAir = true;
		break;
	case CLIMB:
		break;
	case BORN:
		status = BORN;
		Abilities[BORN].isUsing = true;
		Abilities[BORN].frameCount = 0;
		break;
	case DIE:
		if (Abilities[DIE].usable) {
			status = DIE;
			Abilities[DIE].isUsing = true;
			Abilities[DIE].frameCount = 0;
		}
		break;
	}
}

void Hero::charge(Speed _dir) {
	status = CHARGE;
	chargeDir = _dir;
	Abilities[CHARGE].usable = false;
	Abilities[CHARGE].isUsing = true;
	Abilities[CHARGE].frameCount = 0;
	speedX = HERO_CHARGE_SPEED * chargeDir.first;
	speedY = HERO_CHARGE_SPEED * chargeDir.second;
	if (chargeDir.second < 0.0)
		inAir = true;
}
void Hero::die(int how) {

}


void Hero::update() {
	if (status != DIE) {
		// change direction faced
		if (speedX > 0.0) facingRight = true;
		else if (speedX < 0.0) facingRight = false;

		if (Abilities[CHARGE].isUsing) {
			// charge
			speedX -= HERO_CHARGE_ACCEL * chargeDir.first;
			speedY -= HERO_CHARGE_ACCEL * chargeDir.second;
			if (speedX * chargeDir.first < 0) speedX = 0.0;
			if (speedY * chargeDir.second < 0) speedY = 0.0;
			Abilities[CHARGE].frameCount++;
			if (Abilities[CHARGE].frameCount > Abilities[CHARGE].nFrame*Abilities[CHARGE].animation_speed) {
				Abilities[CHARGE].isUsing = false;
				status = STAND;
			}
		}

		// speed Y
		if (inAir) {
			if (!Abilities[CHARGE].isUsing) {
				speedY += HERO_GRAVITY;
				if (speedY >= HERO_SPEED_Y) speedY = HERO_SPEED_Y;
			}
		}
		else speedY = 0;

		if (!Abilities[CHARGE].isUsing) {
			// speed X
			if (key_status[VK_LEFT] && !key_status[VK_RIGHT] && speedX > -HERO_SPEED_X) {
				speedX -= HERO_ACCELX; // press left
			}
			else if (!key_status[VK_LEFT] && key_status[VK_RIGHT] && speedX < HERO_SPEED_X) {
				speedX += HERO_ACCELX; // press right key
			}
			else if (speedX > 0) {
				speedX -= HERO_ACCELX;
				if (speedX < 0) speedX = 0.0;
			}
			else if (speedX < 0) {
				speedX += HERO_ACCELX;
				if (speedX > 0) speedX = 0.0;
			}
		}

		// hit wall
		if (whatWall == 1 && speedX > 0) {
			speedX = 0;
		}
		else if (whatWall == -1 && speedX < 0) {
			speedX = 0;
		}

		// set status
		if (status == WALK && abs(speedX) < 0.1 && abs(speedY) < 0.1) status = STAND;
		else if (status == STAND && abs(speedX) > 0.1 && abs(speedY) < 0.1) status = WALK;

		// position
		setPos(int(posX + speedX), int(posY + speedY));
	}

	// animation
	switch (status) {
	case STAND:
	case WALK:
		Abilities[status].frameCount++;
		Abilities[status].frameCount %= (Abilities[status].nFrame * Abilities[status].animation_speed);
		break;
	case JUMP:
	case BORN:
		Abilities[status].frameCount++;
		if (Abilities[status].frameCount > Abilities[status].nFrame*Abilities[status].animation_speed) {
			Abilities[status].isUsing = false;
			status = STAND;
		}
		break;
	case DIE:
		Abilities[status].frameCount++;
		if (Abilities[status].frameCount > Abilities[status].nFrame*Abilities[status].animation_speed) {
			Abilities[status].isUsing = false;
			status = BORN;
		}
		break;
	}
}

void Hero::render(HDC bmp_buffer, HDC hdc_loadbmp, int cameraX, int cameraY) {
	SelectObject(hdc_loadbmp, img);

	int frameX = 2 * status + (!facingRight);
	int frameY = Abilities[status].frameCount / Abilities[status].animation_speed;

	if (status == STAND) {
		if (inAir) frameY = 1;
		else if (onEdge) frameY = 2;
	}
	/*if (status == CHARGE && Abilities[CHARGE].frameCount > 2) {
		int offset = facingRight ? -HERO_WIDTH : HERO_WIDTH;
		TransparentBlt(
			bmp_buffer, posX - cameraX + offset, posY - cameraY,
			width, height,
			hdc_loadbmp, HERO_BMP_WIDTH*frameX, HERO_BMP_HEIGHT*Abilities[CHARGE].nFrame, HERO_BMP_WIDTH, HERO_BMP_HEIGHT,
			RGB(255, 255, 255)
		);
	}*/
	TransparentBlt(
		bmp_buffer, posX-cameraX, posY-cameraY,
		width, height,
		hdc_loadbmp, HERO_BMP_WIDTH*frameX, HERO_BMP_HEIGHT*frameY, HERO_BMP_WIDTH, HERO_BMP_HEIGHT,
		RGB(255, 255, 255)
	);
	
	//TransparentBlt(bmp_buffer, posX - cameraX, posY - cameraY,width, height,hdc_loadbmp, 0, 0, HERO_BMP_WIDTH, HERO_BMP_HEIGHT,RGB(255, 255, 255));
}

Hero::~Hero()
{
	//DeleteObject(img);
}
