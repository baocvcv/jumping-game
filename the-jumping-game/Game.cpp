#include "stdafx.h"
#include "Game.h"


Game::Game()
{
}

void Game::init() {
	// load all the menus
	for (int i = 0; i < NO_OF_MENUS; i++) {
		int menuId = MENU_INFO[i][0][0];
		int bmpId = bmp_resources[menuId];
		HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(bmpId));
		Menu menu(bitmap, MENU_INFO[i]);
		menus[menuId] = menu;
	}
	exitAnimationOn = false;
	currentSceneId = -1;
	switchScene(MENU_START);
}

void Game::update() {
	if (exitAnimationOn) return;
	if (currentSceneId < 100 || maps.count(currentSceneId) == 0) {
		return;
	}
	int action_id = maps[currentSceneId].update();
	if (action_id != currentSceneId)
		switchScene(action_id);
}

void Game::render(HDC bmp_buffer, HDC hdc_loadbmp) {
	int sceneToRender = currentSceneId;
	if (exitAnimationOn) sceneToRender = lastSceneId;

	if (sceneToRender < 100 && menus.count(sceneToRender) == 1) {
		menus[sceneToRender].render(bmp_buffer, hdc_loadbmp);
		if (exitAnimationOn && menus[sceneToRender].isAnimationFinished()) {
			exitAnimationOn = false;
		}
	}
	else if(sceneToRender >= 100 && maps.count(sceneToRender) == 1){
		maps[sceneToRender].render(bmp_buffer, hdc_loadbmp);
		if (exitAnimationOn && maps[sceneToRender].isAnimationFinished()) {
			exitAnimationOn = false;
		}
	}

}

void Game::mouseEvent(int _x, int _y, bool isClick) {
	mouseX = _x;
	mouseY = _y;

	if (exitAnimationOn) return;
	static int action_id = -1;
	if (currentSceneId < 100 && menus.count(currentSceneId) == 1) {
		if (!mouseLeftButtonDown && isClick) {
			action_id = menus[currentSceneId].eventMouseLeftDown(mouseX, mouseY);
		}
		else if (mouseLeftButtonDown && !isClick) {
			int id2 = menus[currentSceneId].eventMouseLeftUp(mouseX, mouseY);
			if (action_id != -1 && action_id == id2) {
				int sceneId = action_id % 1000;
				switch (action_id / 1000) { // see what action is required
				case 0:
				case 2:
					if(sceneId >= 100 && maps.count(sceneId) == 1)
						maps[sceneId].resetMap();
					break;
				}
				switchScene(sceneId);
				action_id = -1;
			}
		}
	}
	mouseLeftButtonDown = isClick;
}

void Game::mouseMove(int _x, int _y) {
	mouseX = _x;
	mouseY = _y;

	if (exitAnimationOn) return;
	if(currentSceneId < 100 && menus.count(currentSceneId) == 1)
		menus[currentSceneId].eventMouseMove(mouseX, mouseY);
}

void Game::keyEvent(int key, bool pressed) {
	if (key_status[key] ^ pressed){
		key_status[key] = pressed;

		if (exitAnimationOn) return;
		if (currentSceneId < 100 || maps.count(currentSceneId) == 0) { // if not in a stage
			return;
		}

		switch (key) {
		case VK_ESCAPE:
			menus[MENU_PAUSE].setParentMap(currentSceneId);
			switchScene(MENU_PAUSE);
			return;
		}
		maps[currentSceneId].keyEvent(key, pressed);
	}
}

void Game::switchScene(int sceneId) {
	// load map if next scene is a stage
	if (sceneId >= 100 && maps.count(sceneId) == 0) {
		Map mapi(sceneId);
		maps[sceneId] = mapi;
	}

	// start animation for menu (and maps)
	if (currentSceneId == -1) {
		lastSceneId = -1;
		exitAnimationOn = false;
	}else if(currentSceneId <= 100 && menus.count(sceneId) == 1) {
		menus[currentSceneId].animate(2); // exit animation
		exitAnimationOn = true;
		lastSceneId = currentSceneId;
	}
	currentSceneId = sceneId;
	if (currentSceneId <= 100 && menus.count(sceneId) == 1) {
		menus[currentSceneId].animate(1);
	}
}

Game::~Game()
{
}
