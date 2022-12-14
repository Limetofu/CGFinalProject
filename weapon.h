#include "stdafx.h"

// �ʿ��� ����

// �����
// ������ �ӵ�
// ��ź��

typedef struct Stat {
	int full_bullet;
	int max_bullet_count; // �� źâ�� �󸶳�?
	int bullet_count;
	int shooting_count_limit; // basic(handgun) : 20  (�����)

	float damage;
	float reload_speed; 
};

class Weapon {
public:
	Stat Handgun = {9999, 10, 10, 20, 1.0f, 1.0f};
	Stat SMG = {200, 40, 40, 7, 0.5f, 0.7f};
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

		bb.left = xpos - 0.05f;
		bb.right = xpos + 0.05f;
		bb.bottom = ypos - 0.05f;
		bb.top = ypos + 0.05f;

		// ��ǥ ���� �Ÿ� �Ѿ�� ����
				
		if (r > 14) {
			init();
		}
	}

	int check_collide(RECTANGLE zbb) {
		if (zbb.left > bb.right) return false;
		if (zbb.right < bb.left) return false;
		if (zbb.bottom > bb.top) return false;
		if (zbb.top < bb.bottom) return false;
		/*if (num >= 0) return num;
		return false;*/
		return true;
	}

	void remove_bullet() {

	}
};