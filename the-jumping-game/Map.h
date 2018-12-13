#pragma once

#include "the-jumping-game.h"
#include "Hero.h"
#include "Shadow.h"

#define TILE_DIM 25
#define TILE_SRC_DIM 40
#define MAP_GRID_X	350
#define MAP_GRID_Y	100
#define PERIMITER_SIZE 25

#define BAD_BRICK_LAG 8
#define BAD_BRICK_LIMIT 33

#define SUCCESS_WIDTH 500
#define SUCCESS_HEIGHT 300
#define SUCCESS_TIME 50

#define BRICK_REFILL 40

/* Tile type
0-9 normal
	0 - empty
	1 - mid
	2 - top

10-19 static trap
	10 - lower boundary
	11 - static trap, top
	12 - static trap, left
	13 - static trap, right
	14 - static trap, bottom

20-29 special bricks
	22 - stage end

30-39 brick-once
	30 up to 35 is fine, 36-39 disappears

40~49 enable charge
*/

class StartPos{
public:
	int heroX, heroY;
	int mapX, mapY;
	bool activated;
};

class Map
{
public:
	Map();
	Map(int _id, bool _isEnemyOn);
	void setId(int _id) { id = _id; }
	void setEnemy(bool _isEnemyOn) { isEnemyOn = _isEnemyOn; }
	
	int update(); // returns the next map to be rendered
	void render(HDC bmp_buffer, HDC hdc_loadbmp);
	
	void keyEvent(int _key, bool pressed) { hero.keyEvent(_key, pressed); }
	void resetMap();
	bool isAnimationFinished() { return true; }
	
	~Map();

private:
	int id;
	HBITMAP background;
	HBITMAP textures;
	HBITMAP success;

	// map
	HDC mapBuffer;
	bool mapRefresh;
	int **stageMap;
	int mapWidth, mapHeight; // num of tiles
	int sceneWidth, sceneHeight; // num of tiles
	int noStartPos;
	std::vector<StartPos> startPosList; // stores all the starting positions available: starting pos is the block on which the hero stands
	std::map<Coordinates, int> badBricks;
	std::map<Coordinates, int> specialItems;
	int cameraX, cameraY; // camera pos
	int renderX, renderY; // map render start
	int succeed_counter; // stage success animation counter
	bool successOn; // if animation started
	// hero
	Hero hero;
	Coordinates heroPos;
	// enemy
	Shadow shadow;
	bool isEnemyOn;

	// map functions
	int collision_test(); // test collide, returns actions
	void camera_move(); // moves the camera with the hero
	void render_map(HDC);
	void resetHero(bool flag);

	// helpers
	bool isHeroOutOfBounds(); // test if hero goes out of bounds
	int whichBoundary(); // what does this do?
	bool isPointInsideBox(Coordinates point, Coordinates box); // check if point in a box
	double calcDist(Coordinates point, Speed direction); // distance according to speed
	std::vector<int> isAgainstWall(int direction); // up right down left
	bool isOnEdge(); // checks if hero is on edge
	bool coordinateInMap(int x, int y); // checks if coordinates are in map data
};

/*
map file:
-mapwidth, mapheight, NumofRestartPos(with the first being the starting pos)
-(NumOfRestartPos)rows: Restart pos
-map data
*/
