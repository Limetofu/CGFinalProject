#include "stdafx.h"
#include <random>

class Zombie {
public:

	float xpos;
	float ypos;

	float spawn_xpos;
	float spawn_ypos;

	float degree;
	float r;

	const char* state;

	int hp;

	int obj_num;
	int walk_num;
	int walk_count;

	RECTANGLE bb;

	void init() {
		xpos = 20.0;
		ypos = 20.0;
		spawn_xpos = 20.0;
		spawn_ypos = 20.0;

		degree = 0.0f;
		r = 0.005f;

		state = "hide";
		hp = 10;

		walk_count = 0;
		walk_num = 0;
		obj_num = 0;
	}

	void spawn(unsigned int rand_seed) {
		// spawn�� spawn_xpos, ypos ����.
		// xpos = spawn_xpos

		// state�� �÷��̾� ������� ����
		//  1. chase
		// //      => chase �����϶��� obj ���� ���
		//  2. dead -> ���ڱ�
		// //      => timer ����, dead_count++, dead_count�� ���� count�� �Ǹ� �������.
		//  3. hide
		// //      => ���� ��Ÿ���� ���� ����

		// hide -> chase
		srand(rand_seed);

		init();

		if (rand() % 2) { // x�� 13 ����, y�� ����
			if (rand() % 2) spawn_xpos = 13.0f;
			else			spawn_xpos = -13.0f;
			spawn_ypos = rand() % 25 - 12;
		}
		else { // y�� 13 ����, x�� ����
			spawn_xpos = rand() % 25 - 12;
			if (rand() % 2) spawn_ypos = 13.0f;
			else			spawn_ypos = -13.0f;
		}

		xpos = spawn_xpos, ypos = spawn_ypos;
		state = "chase";
		
	}

	void update(float p_x, float p_y) {
		degree = mouse_radian(p_x, p_y, xpos, ypos);

		xpos = spawn_xpos - sin(degree) * r;
		ypos = spawn_ypos - cos(degree) * r;

		spawn_xpos = xpos;
		spawn_ypos = ypos;

		bb.left = xpos - 0.5f;
		bb.right = xpos + 0.5f;
		bb.bottom = ypos - 0.5f;
		bb.top = ypos + 0.5f;
	}
};

