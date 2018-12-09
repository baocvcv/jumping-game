#include "stdafx.h"
#include "Shadow.h"


Shadow::Shadow()
{
	img = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_DEVIL));
	facingRight = true;
	inAction = false;
	letsGo = false;
	posX = 0; posY = 0;
}


Shadow::~Shadow()
{
}

void Shadow::reset() {
	inAction = false;
	letsGo = false;
	posX = 0; posY = 0;
	facingRight = true;
	history.clear();
}

void Shadow::record(Coordinates _pos) {
	if (!letsGo) return;
	history.push_back(_pos);
	if (history.size() > SHADOW_LAG) {
		if (!inAction) inAction = true;
		Coordinates tmp = history.front();
		posX = tmp.first; posY = tmp.second;
		history.erase(history.begin());
	}
}

void Shadow::render(HDC bmp_buffer, HDC hdc_loadbmp, int cameraX, int cameraY) {
	if (!inAction) return;

	SelectObject(hdc_loadbmp, img);

	TransparentBlt(
		bmp_buffer, posX - cameraX, posY - cameraY, SHADOW_WIDTH, SHADOW_HEIGHT,
		hdc_loadbmp, 0, 0, SHADOW_SRC_WIDTH, SHADOW_SRC_HEIGHT,
		RGB(255, 255, 255)
	);
}