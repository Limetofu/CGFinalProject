#include "stdafx.h"
#include <random>

class Zombie {
public:

	float xpos;
	float ypos;

	float spawn_xpos;
	float spawn_ypos;

	const char* state;

	int dead_count;

	RECTANGLE bb;

	void spawn() {
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
		
	}

	void init() {
		
	}

	void update() {
		// ��ġ, bb ������Ʈ
		// state�� main����
		if (state == "chase") {
			bb.left = xpos - 0.5f;
			bb.right = xpos + 0.5f;
			bb.bottom = ypos - 0.5f;
			bb.top = ypos + 0.5f;
		}
	}
};