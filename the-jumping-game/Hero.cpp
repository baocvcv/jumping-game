#include "stdafx.h"
#include "Hero.h"

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
	posX = x;
	posY = y;
	speedX = 0;
	speedY = 0;

	inAir = true;
	whatWall = 0;
}

void Hero::onGround(bool isOn) {
	if (isOn) {
		speedY = 0.0;
		inAir = false;
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

void Hero::die(int how) {
	
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
void Hero::keyEvent() {
	if (key_status[VK_UP] && !key_status[VK_DOWN]) { // press up key
		if (!inAir) {
			regularJump();
		}
	}
	else if (!key_status[VK_UP] && key_status[VK_DOWN]) { // press down key
		
	}
	else {

	}
	if (key_status[VK_LEFT] && !key_status[VK_RIGHT]) { // press left key
		
	}
	else if (!key_status[VK_LEFT] && key_status[VK_RIGHT]) { // press right key
		
	}
	else {
		
	}
}

void Hero::update() {
	// speed
	if (inAir) {
		speedY += HERO_GRAVITY;
		if (speedY >= HERO_SPEED_Y) speedY = HERO_SPEED_Y;
	}
	else speedY = 0;

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
	if (whatWall == 1 && speedX > 0) {
		speedX = 0;
	}
	else if (whatWall == -1 && speedX < 0) {
		speedX = 0;
	}

	// position
	posX += (int)speedX;
	posY += (int)speedY;

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
