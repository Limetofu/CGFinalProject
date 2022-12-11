#include "stdafx.h"

// 필요한 변수

// 연사력
// 재장전 속도
// 장탄수

class Handgun {
	int holding = true;
	float reload_speed;
};

class SMG {
	int holding;

};

class AssaultRifle {
	int holding;

};

class SniperRifle {
	int holding;

};

class Shotgun {
	int holding;

};

class Chainsaw {
	int holding;

};


// 총알을 쏘면,
// 중점과, 마우스 좌표를 지나는 직선의 방정식에 의하여
// 직선을 그림.
// 
// 마우스 좌클릭 시, 총알을 쏘는데, 
// 조건
//  1. 들고있는 무기(weapon_holding)의 연사 count가 다 끝났을 때
//  2. 탄창이 있을 때
//  3. 

class Bullet {
	float xpos;
	float ypos;

	int show;

	RECTANGLE bb;

public:
	void init() {
		xpos = 0.0f;
		ypos = 0.0f;

		show = false;
	}
	void update() {
		bb.left = xpos - 0.05f;
		bb.right = xpos + 0.05f;
		bb.bottom = ypos - 0.05f;
		bb.top = ypos + 0.05f;
	}
};