#include "stdafx.h"
#include "Map.h"

#include <fstream>
#include <iostream>
#include <cmath>

#define DIST(x,y) (pow((x)*(x)+(y)*(y), 0.5))

Map::Map() {

}

Map::Map(int _id, bool _isEnemyOn)
{
	// set id, enemy
	id = _id;
	isEnemyOn = _isEnemyOn;

	// load map
	std::ifstream mapfile;
	mapfile.open(map_files[id]);
	mapfile >> mapWidth >> mapHeight >> noStartPos;
	for (int i = 0; i < noStartPos; i++) {
		StartPos pos;
		mapfile >> pos.heroX >> pos.heroY >> pos.mapX >> pos.mapY;
		pos.activated = false;
		startPosList.push_back(pos);
	}
	startPosList[0].activated = true;

	stageMap = new int*[mapHeight];
	for (int i = 0; i < mapHeight; i++) {
		stageMap[i] = new int[mapWidth];
	}
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			mapfile >> stageMap[y][x];
			if (stageMap[y][x] == 30) {
				badBricks[std::make_pair(x, y)] = 0;
			}
			else if (stageMap[y][x] == 40) {
				specialItems[std::make_pair(x, y)] = 0;
			}
		}
	}
	mapfile.close();
	sceneWidth = WINDOW_WIDTH / TILE_DIM;
	sceneHeight = WINDOW_HEIGHT / TILE_DIM;

	// load textures & bkg
	textures = LoadBitmap(hInst, MAKEINTRESOURCE(texture_resources[id]));
	if (bmp_resources.count(id) == 1) {
		background = LoadBitmap(hInst, MAKEINTRESOURCE(bmp_resources[id]));
	}
	else {
		background = NULL;
	}
	success = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_SUCCESS)); // success pic to be changed?

	// set camera & load hero
	resetHero(false);
}

void Map::resetHero(bool flag) {
	int n = 0;
	double dist = DIST(heroPos.first - startPosList[0].heroX*TILE_DIM, heroPos.second - startPosList[0].heroY*TILE_DIM + HERO_HEIGHT);
	if (flag) {
		heroPos = hero.getPos();
		for (unsigned int i = 1; i < startPosList.size() && startPosList[i].activated; i++) {
			double tmp = DIST(heroPos.first - startPosList[i].heroX*TILE_DIM, heroPos.second - startPosList[i].heroY*TILE_DIM + HERO_HEIGHT);
			if (tmp < dist) {
				dist = tmp;
				n = i;
			}
		}
	}
	// reset the bricks
	if (badBricks.size() != 0) {
		std::map<Coordinates, int>::iterator it = badBricks.begin();
		while (it != badBricks.end()) {
			(*it).second = 0;
			stageMap[(*it).first.second][(*it).first.first] = 30;
			it++;
		}
	}
	if (specialItems.size() != 0) {
		std::map<Coordinates, int>::iterator it = specialItems.begin();
		while (it != specialItems.end()) {
			(*it).second = 0;
			stageMap[(*it).first.second][(*it).first.first] = 40;
			it++;
		}
	}
	cameraX = startPosList[n].mapX * TILE_DIM; 
	cameraY = startPosList[n].mapY * TILE_DIM;
	int posX = startPosList[n].heroX*TILE_DIM;
	int posY = startPosList[n].heroY*TILE_DIM - HERO_HEIGHT;
	hero.reset(posX, posY);
	heroPos = std::make_pair(posX, posY);
	if(isEnemyOn) shadow.reset();
	successOn = false;
}

void Map::resetMap() {
	for (unsigned int i = 1; i < startPosList.size(); i++) {
		startPosList[i].activated = false;
	}
	resetHero(false);
}

/* actions:
0-nothing 1-dies 2-go to stage select
*/
int Map::collision_test() {
	heroPos = hero.getPos();  // relative coordinate to camera

	// test if on map boundary
	if (isHeroOutOfBounds()) {
		return 1;
	}

	Speed heroSpeed = hero.getSpeed();	

	//test if collides
	bool isColliding;
	double dist;
	do {
		dist = 0.0;
		std::vector<Coordinates> border = hero.getBorderNodes(4);
		std::vector<Coordinates> nearbySquares; // relative to camera
		isColliding = false;
		for (unsigned int j = 0; j < border.size(); j++) {
			int xx = border[j].first / TILE_DIM;
			int yy = border[j].second / TILE_DIM;
			if (xx >= 0 && xx < mapWidth && yy >= 0 && yy < mapHeight) {
				int tile_num = stageMap[yy][xx];
				if (tile_num > 0 && tile_num < BAD_BRICK_LIMIT) {
					// handle special tiles that have been touched
					switch (tile_num) {
					case 22: // reached stage end
						PlayWav("SUCCEED", "0", "", NULL);
						successOn = true;
						return 2; // go back to stage select
					case 10: // dies
						return 1; // reset
					}

					isColliding = true;
					int x = border[j].first % TILE_DIM;
					int y = border[j].second % TILE_DIM;
					double d = calcDist(std::make_pair(x, y), heroSpeed);
					if (d > dist) {
						dist = d;
					}
				}
				else if (tile_num == 40) {
					hero.enableAbility(CHARGE);
					specialItems[std::make_pair(xx, yy)] = BAD_BRICK_LAG;
				}
			}
		}
		// move back
		if (dist > 0.001) {
			double d = pow(heroSpeed.first*heroSpeed.first + heroSpeed.second*heroSpeed.second, 0.5);
			double cos = heroSpeed.first / d;
			double sin = heroSpeed.second / d;
			int newPosX = int(heroPos.first - dist * cos);
			int newPosY = int(heroPos.second - dist * sin);
			if (dist > 0.0) { // not collide with anything
				hero.setPos(newPosX, newPosY);
				heroPos = std::make_pair(newPosX, newPosY);
			}
		}
	} while (isColliding);

	// test if caught by shadow
	if (isEnemyOn && shadow.isChasing()) {
		double distX = abs(heroPos.first + HERO_WIDTH / 2.0 - (shadow.getX()+SHADOW_WIDTH/2.0));
		double distY = abs(heroPos.second+HERO_HEIGHT/2.0 - (shadow.getY()+SHADOW_HEIGHT/2.0));
		if (distX < (HERO_WIDTH + SHADOW_WIDTH)/2.0 && distY < (HERO_HEIGHT + SHADOW_HEIGHT)/2.0) {
			return 1;
		}
	}

	// activate startposition if any
	for (unsigned int i = 1; i < startPosList.size(); i++) {
		if (startPosList[i].activated) continue;
		int sx = startPosList[i].heroX * TILE_DIM;
		int hx = heroPos.first;
		if (hx >= sx - HERO_WIDTH) startPosList[i].activated = true;
	}
	
	// set hero status according to relationship with objects
	std::vector<int> res = isAgainstWall(0);
	if (res.size() > 0) { //up
		hero.setSpeedY(0);
		for (unsigned int i = 0; i < res.size(); i++) if (res[i] == 14 || res[i] == 12 || res[i] == 13) return 1;
	}

	res = isAgainstWall(2);
	if (res.size() > 0) { //down
		hero.setOnGround(true);
		if (isOnEdge()) hero.setOnEdge(true);
		else hero.setOnEdge(false);
		for (unsigned int i = 0; i < res.size(); i++) if (res[i] == 11 || res[i] == 12 || res[i] == 13 || res[i] == 10) return 1;
	}
	else {
		hero.setOnGround(false);
		hero.setOnEdge(false);
	}

	res = isAgainstWall(1);
	if (res.size() > 0) { //right
		hero.hitVerticalWall(1);
		for (unsigned int i = 0; i < res.size(); i++) if (res[i] == 12 || res[i] == 11 || res[i] == 14) return 1;
	}
	else {
		res = isAgainstWall(3);
		if (res.size() > 0) { //left
			hero.hitVerticalWall(-1);
			for (unsigned int i = 0; i < res.size(); i++) if (res[i] == 13 || res[i] == 11 || res[i] == 14) return 1;
		}
		else hero.hitVerticalWall(0);
	}

	return 0;
}

void Map::camera_move() {
	heroPos = hero.getPos();
	Speed heroSpeed = hero.getSpeed();

	int relativeX = heroPos.first - cameraX;
	int relativeY = heroPos.second - cameraY;

	// direction x, moves with hero, stops at border
	if (relativeX <= sceneWidth * TILE_DIM * 2 / 5 && heroSpeed.first < 0.0) {
		cameraX += int(heroSpeed.first);
		if (cameraX < TILE_DIM)
			cameraX = TILE_DIM;
	}
	else if (relativeX >= sceneWidth * TILE_DIM * 3 / 5 && heroSpeed.first > 0.0) {
		cameraX += int(heroSpeed.first);//heroPos.first - sceneWidth * TILE_DIM * 3 / 5;
		if (cameraX > (mapWidth - sceneWidth - 1)*TILE_DIM)
			cameraX = (mapWidth - sceneWidth - 1)*TILE_DIM;
	}

	// direction y, moves with hero, stops at border and camera blocks
	if (relativeY <= sceneHeight * TILE_DIM * 2/7 && heroSpeed.second < 0.0) {
		cameraY += int(heroSpeed.second);
		if (cameraY < TILE_DIM)
			cameraY = TILE_DIM;
	}
	else if (relativeY >= sceneHeight * TILE_DIM * 5/7 && heroSpeed.second > 0.0) {
		cameraY += int(heroSpeed.second);//heroPos.second - sceneHeight * TILE_DIM * 3 / 5;
		if (cameraY > (mapHeight - sceneHeight - 1)*TILE_DIM)
			cameraY = (mapHeight - sceneHeight - 1)*TILE_DIM;
	}
}

int Map::update() {
	if (successOn) {
		if (succeed_counter < SUCCESS_TIME) {
			succeed_counter++;
			return id;
		}
		else {
			if(id == STAGE_HELP) return MENU_HELP;
			else return MENU_SELECT_STAGE;
		}
	}
	else {
		// update bad bricks
		if (badBricks.size() != 0) {
			std::map<Coordinates, int>::iterator it = badBricks.begin();
			while (it != badBricks.end()) {
				if ((*it).second > 0) {
					(*it).second++;
					if ((*it).second >= 10 * BAD_BRICK_LAG) {
						(*it).second = 0;
					}
					int x = (*it).first.first;
					int y = (*it).first.second;
					stageMap[y][x] = 30 + (*it).second / BAD_BRICK_LAG;
				}
				it++;
			}
		}
		// update special bricks
		if (specialItems.size() != 0) {
			std::map<Coordinates, int>::iterator it = specialItems.begin();
			while (it != specialItems.end()) {
				if ((*it).second > 0) {
					(*it).second++;
					if ((*it).second >= 10 * BAD_BRICK_LAG) {
						(*it).second = 0;
					}
					int x = (*it).first.first;
					int y = (*it).first.second;
					stageMap[y][x] = 40 + (*it).second / BAD_BRICK_LAG;
				}
				it++;
			}
		}

		hero.update();
		int action = collision_test();
		camera_move();

		if (isEnemyOn) {
			if (!shadow.isChasing() && hero.hasMoved()) shadow.canChase();
			if (action == 0) shadow.record(heroPos);
		}

		Talent status;
		switch (action) {
		case 0:
			return id;
		case 1: // die
			status = hero.getStatus();
			if (status == BORN) {
				resetHero(true);
			}
			else if (status != DIE) { // start dying and reset shadow
				hero.useAbility(DIE, 0, 0);
			}
			return id;
		case 2: // success
			succeed_counter = 0;
		}
		return id;
	}
}

void Map::render(HDC bmp_buffer, HDC hdc_loadbmp) {
	render_map(hdc_loadbmp);
	BitBlt(
		bmp_buffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		mapBuffer, cameraX-renderX, cameraY-renderY, SRCCOPY
	);

	if(isEnemyOn) shadow.render(bmp_buffer, hdc_loadbmp, cameraX, cameraY);
	hero.render(bmp_buffer, hdc_loadbmp, cameraX, cameraY);

	if (successOn) {
		SelectObject(hdc_loadbmp, success);
		BitBlt(
			bmp_buffer, (WINDOW_WIDTH-SUCCESS_WIDTH)/2, (WINDOW_HEIGHT-SUCCESS_HEIGHT)/2, SUCCESS_WIDTH, SUCCESS_HEIGHT,
			hdc_loadbmp, 0, 0, SRCCOPY
		);
	}
}

void Map::render_map(HDC hdc_loadbmp) {
	int x0 = cameraX / TILE_DIM; renderX = x0 * TILE_DIM;
	int y0 = cameraY / TILE_DIM; renderY = y0 * TILE_DIM;
	
	// create buffer that holds the map
	if (mapBuffer == NULL)
		mapBuffer = CreateCompatibleDC(hdc);
	HBITMAP bmp_blank = CreateCompatibleBitmap(hdc, (sceneWidth + 2)*TILE_DIM, (sceneHeight + 2)*TILE_DIM);
	SelectObject(mapBuffer, bmp_blank);

	// draw the background
	if (background != NULL) {
		SelectObject(hdc_loadbmp, background);
		if (id == STAGE_HELP) {
			TransparentBlt(
				mapBuffer, 0, 0, WINDOW_WIDTH + TILE_DIM, WINDOW_HEIGHT + TILE_DIM,
				hdc_loadbmp, renderX, renderY, WINDOW_WIDTH + TILE_DIM, WINDOW_HEIGHT + TILE_DIM,
				SRCCOPY
			);
		}
		else {
			BitBlt(mapBuffer, cameraX - renderX, cameraY - renderY, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_loadbmp, 0, 0, SRCCOPY);
		}
}

	// draw the map
	SelectObject(hdc_loadbmp, textures);
	for (int y = 0; y <= sceneHeight; y++) {
		for (int x = 0; x <= sceneWidth; x++) {
			int tileId = stageMap[y+y0][x+x0];
			int tileX = tileId / 10;
			int tileY = tileId % 10;
			if (tileId > 0) {
				TransparentBlt(
					mapBuffer, x*TILE_DIM, y*TILE_DIM, TILE_DIM, TILE_DIM,
					hdc_loadbmp, tileX * TILE_SRC_DIM, tileY * TILE_SRC_DIM, TILE_SRC_DIM, TILE_SRC_DIM,
					RGB(255, 255, 255)
				);
			}
		}
	}
	DeleteObject(bmp_blank);
}

Map::~Map()
{
	
}

bool Map::isHeroOutOfBounds() {
	int x1 = heroPos.first;
	int y1 = heroPos.second;
	int x2 = x1 + HERO_WIDTH;
	int y2 = y1 + HERO_HEIGHT;

	if (x2 < 0 || y2 < 0 || x2 > mapWidth*TILE_DIM || y2 > mapHeight*TILE_DIM) {
		return true;
	}
	return false;
}

int Map::whichBoundary() {
	int x = heroPos.first + HERO_WIDTH / 2 - cameraX;
	int y = heroPos.second + HERO_HEIGHT / 2 - cameraY;

	if (x <= 0 || x >= WINDOW_WIDTH || y <= 0 || y >= WINDOW_HEIGHT) {
		int xx = (x+cameraX) / TILE_DIM;
		if (xx < 0) 
			xx = 0;
		else if (xx >= mapWidth) 
			xx = mapWidth - 1;

		int yy = (y+cameraY) / TILE_DIM;
		if (yy < 0) 
			yy = 0;
		else if (yy >= mapHeight) 
			yy = mapHeight - 1;

		return stageMap[yy][xx];
	}
	return -1;
}

bool Map::isPointInsideBox(Coordinates point, Coordinates box) {
	int x = point.first;
	int y = point.second;
	int boxX = box.first * TILE_DIM;
	int boxY = box.second * TILE_DIM;
	if (x >= boxX &&
		x <= boxX + TILE_DIM - 1 &&
		y >= boxY &&
		y <= boxY + TILE_DIM - 1)
		return true;
	return false;
}

double Map::calcDist(Coordinates point, Speed direction) {
	double d = pow(direction.first*direction.first + direction.second*direction.second, 0.5);
	double cos = direction.first / d;
	double sin = direction.second / d;

	double dist = TILE_DIM * 2.0;
	if (direction.first != 0.0) {
		double x = (point.first - (-1)) / cos;
		if (x < dist && x >= 0) dist = x;
		x = (point.first - TILE_DIM) / cos;
		if (x < dist && x >= 0) dist = x;
	}
	if (direction.second != 0.0) {
		double x = (point.second - (-1)) / sin;
		if (x < dist && x >= 0) dist = x;
		x = (point.second - TILE_DIM) / sin;
		if (x < dist && x >= 0) dist = x;
	}
	return dist;
}

std::vector<int> Map::isAgainstWall(int direction) {
	std::vector<Coordinates> borderNodes = hero.getBorderNodes(direction);
	int increment[4][2] = { {0,-1},{1,0},{0,1},{-1,0} };
	std::vector<int> res;

	for (unsigned int i = 0; i < borderNodes.size(); i++) {
		int x = borderNodes[i].first;
		int y = borderNodes[i].second;
		int xx1 = x / TILE_DIM;
		int xx2 = (x + increment[direction][0]) / TILE_DIM;
		int yy1 = y / TILE_DIM;
		int yy2 = (y + increment[direction][1]) / TILE_DIM;
		
		if (coordinateInMap(xx1, yy1) && coordinateInMap(xx2, yy2)) {
			if (stageMap[yy1][xx1] == 0 && stageMap[yy2][xx2] > 0 && stageMap[yy2][xx2] < BAD_BRICK_LIMIT) {
				if (direction == 2) { // if checking downwards
					if (stageMap[yy2][xx2] == 30 && badBricks[std::make_pair(xx2, yy2)] == 0) { // if bad brick is not triggered
						badBricks[std::make_pair(xx2, yy2)] = 1;
						int i = 1;
						while (stageMap[yy2][xx2 + i] == 30) { // trigger nearby bricks
							badBricks[std::make_pair(xx2 + i, yy2)] = 1; i++;
						}
						i = 1;
						while (stageMap[yy2][xx2 - i] == 30) { // trigger nearby bricks
							badBricks[std::make_pair(xx2 - i, yy2)] = 1; i++;
						}
					}
				}
				res.push_back(stageMap[yy2][xx2]);
			}
		}
	}
	return res;
}

bool Map::isOnEdge() {
	std::vector<Coordinates> borderNodes = hero.getBorderNodes(2);
	{
		int i = 1;
		int x = borderNodes[i].first; int y = borderNodes[i].second;
		int xx1 = x / TILE_DIM; int yy1 = (y+1) / TILE_DIM;
		if (coordinateInMap(xx1, yy1)) {
			if (stageMap[yy1][xx1] == 0) {
				return true;
			}
		}
	}
	{
		int i = borderNodes.size() - 2;
		int x = borderNodes[i].first; int y = borderNodes[i].second;
		int xx1 = x / TILE_DIM; int yy1 = (y + 1) / TILE_DIM;
		if (coordinateInMap(xx1, yy1)) {
			if (stageMap[yy1][xx1] == 0) {
				return true;
			}
		}
	}
	return false;
}

bool Map::coordinateInMap(int x, int y) {
	if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
		return true;
	}
	return false;
}