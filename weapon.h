#include "stdafx.h"

// �ʿ��� ����

// �����
// ������ �ӵ�
// ��ź��

typedef struct Stat {
	int first_given_bullet;
	int max_bullet_count; // �� źâ�� �󸶳�?
	int bullet_count;
	int shooting_count_limit; // basic(handgun) : 20
	
	float damage;
	float reload_speed; 
};

class Weapon {
public:
	Stat Handgun = {999, 10, 10, 20, 1.0f, 1.0f};
	Stat SMG = {};
	Stat AssaultRifle = {};
	Stat SniperRifle = {};
	Stat Shotgun = {};

	Stat holding = Handgun;

	const char* type;

	void use(const char* w) {
		if (w == "Handgun") {
			holding = Handgun;
		}
		else if (w == "SMG") {
			holding = SMG;
		}
		else if (w == "AssaultRifle") {
			holding = AssaultRifle;
		}
		else if (w == "SniperRifle") {
			holding = SniperRifle;
		}
		else if (w == "Shotgun") {
			holding = Shotgun;
		}
		type = w;
	}

	void shoot() {

	}
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
public:
	float start_xpos;
	float start_ypos;

	float xpos;
	float ypos;

	float degree;
	float r; // ������

	int show;
	int penetrate;

	RECTANGLE bb;

	void init() {
		start_xpos = 0.0f;
		start_ypos = 0.0f;
		xpos = 0.0f;
		ypos = 0.0f;

		show = false;
		degree = 0.0f;
		r = 0.0f;
	}

	void gen(float x1, float y1, float deg, int pen = false) {
		show = true;
		penetrate = pen;

		start_xpos = x1, start_ypos = y1;
		xpos = start_xpos, ypos = start_ypos;

		degree = deg;
		r = 0.0f;
	}

	void move(float x_move, float y_move) {
		r += 0.2f;
		update(x_move, y_move);
	}

	void update(float x_move, float y_move) {
		xpos = start_xpos + cos(double(degree)) * r;
		ypos = start_ypos + sin(double(degree)) * r;

		bb.left = xpos + x_move - 0.05f;
		bb.right = xpos + x_move + 0.05f;
		bb.bottom = ypos - y_move - 0.05f;
		bb.top = ypos - y_move + 0.05f;

		// ��ǥ ���� �Ÿ� �Ѿ�� ����

		if (r > 14) {
			init();
		}
	}
};