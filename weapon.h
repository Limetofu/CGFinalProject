#include "stdafx.h"

// 필요한 변수

// 연사력
// 재장전 속도
// 장탄수

typedef struct Stat {
	int full_bullet;
	int max_bullet_count; // 한 탄창에 얼마나?
	int bullet_count;
	int shooting_count_limit; // basic(handgun) : 20  (연사력)

	float damage;
	float reload_speed;
};

class Weapon {
public:
	Stat Handgun = {1000, 10, 10, 20, 0.2f, 1.0f};
	Stat SMG = {5, 40, 40, 3, 0.1f, 0.7f};
	Stat AssaultRifle = {5, 30, 30, 9, 0.5f, 1.0f};
	Stat SniperRifle = {3, 7, 7, 120, 10.0f, 1.5f};
	Stat Shotgun = {4, 5, 5, 100, 2.5f, 3.0f};

	Stat holding = Handgun;

	const char* type;
	float reload_count = 0.0f;
	int reloading = false;

	void use(const char* w) {
		type = w;
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
	}

	float reload() {
		if (holding.full_bullet == 0) {
			reloading = -1;
			return 0;
		}
		else {
			reload_count = holding.reload_speed * 200;
			reloading = true;
			return reload_count;
		}
	}
	void update() {
		if (reloading) {
			if (reload_count <= 0) {
				reload_count = 0;
				holding.full_bullet--;
				holding.bullet_count = holding.max_bullet_count;
				reloading = false;
			}
			else {
				reload_count -= 1;
			}
		}
	}
};

class Bullet {
public:
	float start_xpos;
	float start_ypos;

	float xpos;
	float ypos;

	float degree;
	float r; // 반지름

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

		bb.left = xpos - 0.1f;
		bb.right = xpos + 0.1f;
		bb.bottom = ypos - 0.1f;
		bb.top = ypos + 0.1f;

		// 좌표 일정 거리 넘어서면 삭제
				
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
		init();
	}
};