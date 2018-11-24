#pragma once

#include "the-jumping-game.h"
#include "Hero.h"

#define TILE_DIM 32
#define MAP_GRID_X	32
#define MAP_GRID_Y	24
#define PERIMITER_SIZE 25

extern int Perimiter[PERIMITER_SIZE][2];
/* Tile type
0 - empty
1 - normal block

*/

class StartPos{
public:
	int heroX, heroY;
	int mapX, mapY;
};

class Map
{
public:
	Map();
	Map(int _id);
	void setId(int _id) { id = _id; }

	// manipulate hero
	void keyEvent() { hero.keyEvent(); }
	void resetHero();

	

	void update();
	void render(HDC bmp_buffer, HDC hdc_loadbmp);
	~Map();

//TODO: camera follow and restart
private:
	int id;
	Hero hero;
	Coordinates heroPos;
	HBITMAP background;
	HBITMAP textures;
	int **stageMap;
	int mapWidth, mapHeight; // num of tiles
	int sceneWidth, sceneHeight; // num of tiles
	int noStartPos;
	std::vector<StartPos> startPosList; // stores all the starting positions available: starting pos is the block on which the hero stands

	// status
	int cameraX, cameraY;

	void collision_test();
	bool isHeroOutOfBounds();
	int whichBoundary();
	bool isPointInsideBox(Coordinates point, Coordinates box);
	double calcDist(Coordinates point, Speed direction);
	bool isAgainstWall(int direction); // up right down left

	bool coordinateInMap(int x, int y); // checks if coordinates are in map data
};

/*
map file:
-mapwidth, mapheight, NumofRestartPos(with the first being the starting pos)
-(NumOfRestartPos)rows: Restart pos
-map data
*/

/*
tile info:



*/
