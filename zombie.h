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