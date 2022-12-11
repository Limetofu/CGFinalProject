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

	const char* weapon_holding;
	int weapon_have[7] = { 0, };

	int obj_num;
	int walk_num;
	int walk_count;

	void init() {
		xdir = 0;
		ydir = 0;

		x_move = 0.0f;
		y_move = 0.0f;

		max_hp = 15;
		hp = 15;
		hp_rezen_speed = 1.0f;

		velocity = 1.0f;
		reload_speed = 1.0f;
		weapon_holding = "sr";

		weapon_have[0] = true;
		for (int i = 1; i < 7; i++) {
			weapon_have[i] = 0;
		}

		obj_num = 4;
		walk_num = 0;
		walk_count = 0;
	}
};