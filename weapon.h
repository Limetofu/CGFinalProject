#include "stdafx.h"

// �ʿ��� ����

// �����
// ������ �ӵ�
// ��ź��

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


// �Ѿ��� ���,
// ������, ���콺 ��ǥ�� ������ ������ �����Ŀ� ���Ͽ�
// ������ �׸�.
// 
// ���콺 ��Ŭ�� ��, �Ѿ��� ��µ�, 
// ����
//  1. ����ִ� ����(weapon_holding)�� ���� count�� �� ������ ��
//  2. źâ�� ���� ��
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