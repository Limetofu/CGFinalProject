#pragma once
#include "stdafx.h"

class Player {
public:
	char dir; // W, A, S, D
	int max_hp;
	int hp;
	float hp_rezen_speed;
	
	float velocity;
	float reload_speed;

	const char* weapon_holding;
	int weapon_have[7] = { 0, };

	void init() {
		dir = 'x';
		max_hp = 15;
		hp = 15;
		hp_rezen_speed = 1.0f;

		velocity = 1.0f;
		reload_speed = 1.0f;
		weapon_holding = "handgun";

		weapon_have[0] = true;
		for (int i = 1; i < 7; i++) {
			weapon_have[i] = 0;
		}
	}
};