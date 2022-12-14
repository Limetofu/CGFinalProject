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

	int hurt_bullet_num;

	int knockbacking;
	int knockback_count;

	int num;

	RECTANGLE bb;

	void init() {
		xpos = 200.0;
		ypos = 200.0;
		spawn_xpos = 200.0;
		spawn_ypos = 200.0;

		degree = 0.0f;
		r = 0.005f;

		state = "hide";
		hp = 3;

		walk_count = 0;
		walk_num = 0;
		obj_num = 0;
		knockbacking = false;
		knockback_count = 0;

		num = -1;
	}

	void spawn(unsigned int rand_seed, int n) {
		// spawn�� spawn_xpos, ypos ����.
		// xpos = spawn_xpos

		// state�� �÷��̾� ������� ����
		//  1. chase
		// //      => chase �����϶��� obj ���� ���
		//  2. dead -> ���ڱ�
		// //      => timer ����, dead_count++, dead_count�� ���� count�� �Ǹ� �������.
		//  3. hide
		// //      => ���� ��Ÿ���� ���� ����

		srand(rand_seed);
		init();

		num = n;

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

		if (knockbacking) {
			xpos = spawn_xpos + sin(degree) * r;
			ypos = spawn_ypos + cos(degree) * r;
			knockback_count--;
			if (knockback_count <= 0) {
				knockback_count = 0;
				knockbacking = false;
			}
		}
		else {
			xpos = spawn_xpos - sin(degree) * r;
			ypos = spawn_ypos - cos(degree) * r;
		}

		spawn_xpos = xpos;
		spawn_ypos = ypos;

		bb.left = xpos - p_x - 0.5f;
		bb.right = xpos - p_x + 0.5f;
		bb.bottom = -ypos + p_y - 0.5f;
		bb.top = -ypos + p_y + 0.5f;
	}

	void knockback(int bullet_num, float dmg) {
		knockbacking = true;
		knockback_count = 5;

		// �Ѿ� ��ȣ�� ���ܾ� ��.
		// �ٽ� ������ �ȵ�!
		hurt_bullet_num = bullet_num;

		hp -= dmg;
		if (hp <= 0) {
			die();
		}
	}

	void die() {
		state = "dead";
	}
};