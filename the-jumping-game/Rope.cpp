#include "stdafx.h"
#include "Rope.h"


Rope::Rope()
{
	isUsed = false;
	isAnchored = false;
}

void Rope::setStartPos(int _x, int _y) {
	startPos.first = _x;
	startPos.second = _y;
}

void Rope::useRope(Speed _dir) {
	dir = _dir;
}

void Rope::retrieveRope() {

}

void Rope::drawRope(HDC hdc_membuffer, HDC hdc_loadbmp) {

}

Rope::~Rope()
{
}
