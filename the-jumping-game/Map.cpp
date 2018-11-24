#include "stdafx.h"
#include "Map.h"

#include <fstream>
#include <iostream>
#include <cmath>

#define DIST(x,y) (pow(x*x+y*y, 0.5))

using namespace std;

int Perimiter[PERIMITER_SIZE][2] = { {-2,-2}, {-2,-1}, {-2, 0}, {-2, 1}, {-2, 2},
									 {-1,-2}, {-1,-1}, {-1, 0}, {-1, 1}, {-1, 2},
									 { 0,-2}, { 0,-1}, { 0, 0}, { 0, 1}, { 0, 2},
									 { 1,-2}, { 1,-1}, { 1, 0}, { 1, 1}, { 1, 2},
									 { 2,-2}, { 2,-1}, { 2, 0}, { 2, 1}, { 2, 2} };


Map::Map() {

}

Map::Map(int _id)
{
	// set id
	id = _id;

	// load map
	std::ifstream mapfile;
	mapfile.open(map_files[id]);
	mapfile >> mapWidth >> mapHeight >> noStartPos;
	for (int i = 0; i < noStartPos; i++) {
		StartPos pos;
		mapfile >> pos.heroX >> pos.heroY >> pos.mapX >> pos.mapY;
		startPosList.push_back(pos);
	}
	stageMap = new int*[mapHeight];
	for (int i = 0; i < mapHeight; i++) {
		stageMap[i] = new int[mapWidth];
	}
	for (int y = 0; y < mapHeight; y++){
		for (int x = 0; x < mapWidth; x++) {
			mapfile >> stageMap[y][x];
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

	// set camera & load hero
	cameraX = startPosList[0].mapX;
	cameraY = startPosList[0].mapY;
	int posX = (startPosList[0].heroX - cameraX)*TILE_DIM;
	int posY = (startPosList[0].heroY - cameraY)*TILE_DIM - HERO_HEIGHT;
	hero.setPos(posX, posY);
	heroPos = make_pair(posX, posY);
}

void Map::resetHero() {
	Coordinates heroPos = hero.getPos();
	double dist = DIST(heroPos.first - startPosList[0].heroX, heroPos.second - startPosList[0].heroY);
	int n = 0;
	for (unsigned int i = 1; i < startPosList.size(); i++) {
		double tmp = DIST(heroPos.first - startPosList[i].heroX, heroPos.second - startPosList[i].heroY);
		if (tmp < dist) {
			dist = tmp;
			n = i;
		}
	}
	int posX = (startPosList[n].heroX - cameraX)*TILE_DIM;
	int posY = (startPosList[n].heroY - cameraY)*TILE_DIM - HERO_HEIGHT;
	hero.reset(posX, posY);
	heroPos = make_pair(posX, posY);
}

void Map::collision_test() {
	heroPos = hero.getPos();  // relative coordinate to camera

	// test if on map boundary
	if (isHeroOutOfBounds()) {
		resetHero();
		return;
	}
	switch (whichBoundary())
	{
	case 0:
		resetHero();
		return;
	default:
		break;
	}

	//test if collides
	std::vector<Coordinates> border = hero.getBorderNodes(4);
	std::vector<Coordinates> nearbySquares; // relative to camera
	Speed heroSpeed = hero.getSpeed();

	int heroSquareX = (heroPos.first + HERO_WIDTH/2) / TILE_DIM; // relative to camera
	int heroSquareY = (heroPos.second + HERO_HEIGHT/2) / TILE_DIM;
	for (int i = 0; i < PERIMITER_SIZE; i++) {
		nearbySquares.push_back(std::make_pair(heroSquareX + Perimiter[i][0], heroSquareY + Perimiter[i][1]));
	}
	int deepestPoint = 0;
	int deepestBox;
	double dist = 0.0;
	for (int i = 0; i < PERIMITER_SIZE; i++) {
		int xx = nearbySquares[i].first + cameraX;
		int yy = nearbySquares[i].second + cameraY;

		if (xx >= 0 && xx < mapWidth && yy >= 0 && yy < mapHeight) {
			if (stageMap[yy][xx] == 0) continue;
			for (unsigned int j = 0; j < border.size(); j++) {
				if (isPointInsideBox(border[j], nearbySquares[i])) {
					int x = border[j].first - nearbySquares[i].first * TILE_DIM;
					int y = border[j].second - nearbySquares[i].second * TILE_DIM;
					double d = calcDist(std::make_pair(x, y), heroSpeed);
					if (d > dist) {
						dist = d;
						deepestPoint = j;
						deepestBox = i;
					}
				}
			}
		}
	}

	double d = pow(heroSpeed.first*heroSpeed.first + heroSpeed.second*heroSpeed.second, 0.5);
	double cos = heroSpeed.first / d;
	double sin = heroSpeed.second / d;
	int newPosX = heroPos.first - int(dist * cos);
	int newPosY = heroPos.second - int(dist * sin);
	if (dist > 0.0) { // not collide with anything
		hero.setPos(newPosX, newPosY);
		heroPos = make_pair(newPosX, newPosY);
	}

	// set hero status according to relationship with objects
	int contactX = newPosX + Nodes_Hero[deepestPoint][0];
	int contactY = newPosY + Nodes_Hero[deepestPoint][1];
	/*
	int borderX = nearbySquares[deepestBox].first * TILE_DIM; // left most border
	int borderY = nearbySquares[deepestBox].second * TILE_DIM; // up most border

	if (contactX == borderX-1) { // if hit wall
		hero.hitVerticalWall(1);
	}
	else if (contactX == borderX + TILE_DIM) {
		hero.hitVerticalWall(-1);
	}
	else {
		hero.hitVerticalWall(0); // does not hit wall
	}
	if (contactY == borderY + TILE_DIM) { // hit wall
		hero.setSpeedY(0);
	}

	// checks if on the ground
	bool isOn = false;
	for (int i = 2; i < 4; i++) {
		int boxX = (newPosX + Borders[i][0]) / TILE_DIM + cameraX;
		int boxY = (newPosY + Borders[i][1] + 2) / TILE_DIM + cameraY;
		if(boxY >=0 && boxY < mapWidth && boxX >= 0 && boxX < mapWidth)
			if (stageMap[boxY][boxX] > 0) 
				isOn = true;
	}
	hero.onGround(isOn);
	*/

	if (isAgainstWall(0)) {
		hero.setSpeedY(0);
	}
	if (isAgainstWall(2)) {
		hero.onGround(true);
	}
	else {
		hero.onGround(false);
	}
	if (isAgainstWall(1)) {
		hero.hitVerticalWall(1);
	}
	else if (isAgainstWall(3)) {
		hero.hitVerticalWall(-1);
	}
	else {
		hero.hitVerticalWall(0);
	}
}

void Map::update() {
	hero.update();
	collision_test();
}

void Map::render(HDC bmp_buffer, HDC hdc_loadbmp) {
	if (background != NULL) {
		SelectObject(hdc_loadbmp, background);
		BitBlt(bmp_buffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_loadbmp, 0, 0, SRCCOPY);
	}
	SelectObject(hdc_loadbmp, textures);
	for (int y = 0; y < sceneHeight; y++) {
		for (int x = 0; x < sceneWidth; x++) {
			int tileId = stageMap[y + cameraY][x + cameraX];
			if (tileId > 0) {
				TransparentBlt(
					bmp_buffer, x*TILE_DIM, y*TILE_DIM, TILE_DIM, TILE_DIM,
					hdc_loadbmp, 0, tileId * TILE_DIM, TILE_DIM, TILE_DIM,
					RGB(255, 255, 255)
				);
			}
		}
	}
	hero.render(bmp_buffer, hdc_loadbmp);
}

Map::~Map()
{
	
}

bool Map::isHeroOutOfBounds() {
	int x1 = heroPos.first;
	int y1 = heroPos.second;
	int x2 = x1 + HERO_WIDTH;
	int y2 = y1 + HERO_HEIGHT;

	if (x2 < 0 || y2 < 0 || x1 > WINDOW_WIDTH || y1 > WINDOW_HEIGHT) {
		return true;
	}
	return false;
}

int Map::whichBoundary() {
	int x = heroPos.first + HERO_WIDTH / 2;
	int y = heroPos.second + HERO_HEIGHT / 2;


	if (x <= 0 || x >= WINDOW_WIDTH || y <= 0 || y >= WINDOW_HEIGHT) {
		int xx = x / TILE_DIM + cameraX;
		if (xx < 0) 
			xx = 0;
		else if (xx >= mapWidth) 
			xx = mapWidth - 1;

		int yy = y / TILE_DIM + cameraY;
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

bool Map::isAgainstWall(int direction) {
	vector<Coordinates> borderNodes = hero.getBorderNodes(direction);
	int increment[4][2] = { {0,-1},{1,0},{0,1},{-1,0} };

	for (unsigned int i = 0; i < borderNodes.size(); i++) {
		int x = borderNodes[i].first;
		int y = borderNodes[i].second;
		int xx1 = x / TILE_DIM + cameraX;
		int xx2 = (x + increment[direction][0]) / TILE_DIM + cameraX;
		int yy1 = y / TILE_DIM + cameraY;
		int yy2 = (y + increment[direction][1]) / TILE_DIM + cameraY;
		
		if (coordinateInMap(xx1, yy1) && coordinateInMap(xx2, yy2)) {
			if (stageMap[yy1][xx1] == 0 && stageMap[yy2][xx2] > 0) {
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

