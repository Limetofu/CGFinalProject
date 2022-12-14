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

	RECTANGLE bb;

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
		money = 0;

		obj_num = 4;
		walk_num = 0;
		walk_count = 0;
	}
};