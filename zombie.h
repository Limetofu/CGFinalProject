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

typedef struct Point {
	float x;
	float y;
} Point;

typedef struct Line {
	Point p1;
	Point p2;
} Line;


int ccw(Point p1, Point p2, Point p3) {
	float cross_product = (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);

	if (cross_product > 0) {
		return 1;
	}
	else if (cross_product < 0) {
		return -1;
	}
	else {
		return 0;
	}
}

int comparator(Point left, Point right) {
	int ret;
	if (left.x == right.x) {
		ret = (left.y <= right.y);
	}
	else {
		ret = (left.x <= right.x);
	}
	return ret;
}

void swap(Point* p1, Point* p2) {
	Point temp;
	temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}

int LineIntersection(Line l1, Line l2) {
	int ret;
	int l1_l2 = ccw(l1.p1, l1.p2, l2.p1) * ccw(l1.p1, l1.p2, l2.p2);
	int l2_l1 = ccw(l2.p1, l2.p2, l1.p1) * ccw(l2.p1, l2.p2, l1.p2);

	if (l1_l2 == 0 && l2_l1 == 0) {
		if (comparator(l1.p2, l1.p1)) swap(&l1.p1, &l1.p2);
		if (comparator(l2.p2, l2.p1)) swap(&l2.p1, &l2.p2);

		ret = (comparator(l2.p1, l1.p2)) && (comparator(l1.p1, l2.p2));
	}

	else {
		ret = (l1_l2 <= 0) && (l2_l1 <= 0);
	}
	return ret;
}

int CollideBulletZombie(float sx1, float sx2, float sy1, float sy2, 
						float zx1, float zx2, float zy1, float zy2) {

	return LineIntersection({ {sx1, sy1}, {sx2, sy2} }, { {zx1, zy1}, {zx2, zy2} });
}