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

	currentSceneId = MENU_START;
}

void Game::update() {
	if (maps.count(currentSceneId) == 0) {
		return;
	}
	maps[currentSceneId].update();
}

void Game::render(HDC bmp_buffer, HDC hdc_loadbmp) {
	if (menus.count(currentSceneId) == 1) {
		menus[currentSceneId].render(bmp_buffer, hdc_loadbmp);
	}
	else if(maps.count(currentSceneId) == 1){
		maps[currentSceneId].render(bmp_buffer, hdc_loadbmp);
	}
}

void Game::mouseEvent(int _x, int _y, bool isClick) {
	mouseX = _x;
	mouseY = _y;

	if (currentSceneId < 10) {
		if (!mouseLeftButtonDown && isClick) {
			int id = menus[currentSceneId].eventMouseLeftClick(mouseX, mouseY);
			if (id != -1) {
				switchScene(id);
			}
		}
		else {
			menus[currentSceneId].eventMouseMove(mouseX, mouseY);
		}
	}
	mouseLeftButtonDown = isClick;
}

void Game::keyEvent(int key, bool pressed) {
	key_status[key] = pressed;
	maps[currentSceneId].keyEvent();
}

void Game::switchScene(int sceneId) {
	currentSceneId = sceneId;
	if (sceneId >= 100) {
		Map mapi(sceneId);
		maps[sceneId] = mapi;
	}
}

Game::~Game()
{
}
