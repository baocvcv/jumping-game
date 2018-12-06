#pragma once
#include <vector>

#include "the-jumping-game.h"

#define ROPE_ANIMATION_LEN 5
#define ROPE_LENGTH 100

class Rope
{
public:
	Rope();
	~Rope();

	void setStartPos(int x, int y);
	
	void useRope(Speed dir);
	void retrieveRope();

	void drawRope(HDC hdc_membufferl, HDC hdc_loadbmp);


	Coordinates startPos;
	Coordinates endPos;
	Speed dir;
	bool isUsed;
	bool isAnchored;
};

