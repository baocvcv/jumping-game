#pragma once

#include <map>

#include "the-jumping-game.h"
#include "Map.h"
#include "Menu.h"
#include "Button.h"
#include "Hero.h"

using namespace std;

class Game
{
public:
	Game();
	~Game();
	
	void init();

	void update(); // timer event, asks map to update itself
	void render(HDC bmp_buffer, HDC hdc_loadbmp); // called by paint() in main

	void mouseEvent(int x, int y, bool isDown);
	void mouseMove(int x, int y);
	void keyEvent(int key, bool pressed);
	
private:
	map<int, Menu> menus;
	map<int, Map> maps;

	int currentSceneId; // 0~99 is menu, 100-199 is game stage
	int lastSceneId;
	bool exitAnimationOn;

	void switchScene(int sceneId);
};
