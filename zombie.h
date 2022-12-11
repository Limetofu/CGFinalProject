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
		// spawn시 spawn_xpos, ypos 지정.
		// xpos = spawn_xpos

		// state를 플레이어 따라오게 지정
		//  1. chase
		// //      => chase 상태일때만 obj 파일 출력
		//  2. dead -> 핏자국
		// //      => timer 설정, dead_count++, dead_count가 일정 count가 되면 사라지게.
		//  3. hide
		// //      => 아직 나타나지 않은 상태

		// hide -> chase
		
	}

	void init() {
		
	}

	void update() {
		// 위치, bb 업데이트
		// state는 main에서
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