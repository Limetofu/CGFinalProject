#pragma once
#include "stdafx.h"

class Player {
public:
	int xdir;
	int ydir;

	float face_dir_radian;

	float x_move;
	float y_move;

	int max_hp;
	int hp;
	float hp_rezen_speed;
	
	float velocity;
	float reload_speed;

	int obj_num;
	int walk_num;
	int walk_count;

	int money;

	const char* state;
	int hit_count;

	RECTANGLE bb;

	void init() {
		xdir = 0;
		ydir = 0;

		x_move = 0.0f;
		y_move = 0.0f;

		max_hp = 30;
		hp = 31;
		hp_rezen_speed = 1.0f;

		velocity = 1.0f;
		reload_speed = 1.0f;
		money = 0;

		obj_num = 4;
		walk_num = 0;
		walk_count = 0;

		state = "idle";
		hit_count = 0;

		bb.left = -0.2f;
		bb.right = 0.2f;
		bb.bottom = -0.2f;
		bb.top = 0.2f;
	}
	
	void hit() {
		if (state == "idle") {
			//hp--;
			state = "hit";
			hit_count = 30;
			if (hp <= 0) {
				die();
			}
		}
	}

	void update() {
		if (state == "hit") {
			if (hit_count <= 0) {
				hit_count = 0;
				state = "idle";
				printf("idled\n");
			}
			else {
				hit_count--;
			}
		}
	}
	
	void die() {
		state = "dead";
		init();
	}

	int check_collide(RECTANGLE zbb) {
		if (zbb.left > bb.right) return false;
		if (zbb.right < bb.left) return false;
		if (zbb.bottom > bb.top) return false;
		if (zbb.top < bb.bottom) return false;
		return true;
	}
};