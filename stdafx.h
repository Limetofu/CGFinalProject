#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")

struct POS {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void InitBuffer();
void InsertRectanglePos();
void InsertPlayerObj();
void InsertWeaponObj();
void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void PassiveMotion(int x, int y);

GLfloat mouse_radian(double x1, double y1, double x2, double y2);
void DrawPlayer(glm::mat4 TR, unsigned int modelLocation);
void DrawFloor(glm::mat4 TR, unsigned int modelLocation);
void DrawWeapon(glm::mat4 TR, unsigned int modelLocation);

void DrawHandgun(unsigned int modelLocation);
void DrawSMG(unsigned int modelLocation);
void DrawAssaultRifle(unsigned int modelLocation);
void DrawSniperRifle(unsigned int modelLocation);
void DrawShotgun(unsigned int modelLocation);
void DrawChainsaw(unsigned int modelLocation);

void SetCamera();
void SetProjection();
void SetLight();
void InsertTexture(int textures_num, const char* path);
void InitTexture();