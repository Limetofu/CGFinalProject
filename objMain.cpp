#define _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include "objRead.h"
#include "player.h"
#include "texture.h"
#include "pos.h"

using namespace std;

GLuint g_window_w = 1000;
GLuint g_window_h = 1000;

int g_window_middle_x = g_window_w / 2;
int g_window_middle_y = g_window_h / 2;

GLuint VAO_rectangle;
GLuint VBO_rectangle;

GLuint VAO[10];
GLuint VBO_position[10];
GLuint VBO_normal[10];
GLuint VBO_texture[10];

GLuint VAO_weapon[7];
GLuint VBO_weapon_position[7];
GLuint VBO_weapon_normal[7];
GLuint VBO_weapon_texture[7];

GLuint VAO_object[5];
GLuint VBO_object_position[5];
GLuint VBO_object_normal[5];
GLuint VBO_object_texture[5];

GLuint textures[10];

bool left_button = 0;
bool right_button = 0;

int mouse_xpos = 0;
int mouse_ypos = 0;

// 직각 / 원근 투영
int show_ortho = false;

// 솔리드 / 와이어 모델
int print_solid = true;

const int num_vertices = 3;
const int num_triangles = 1;

const char* world_time = "night";

glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

// camera basic pos = 0, 8, 5
POS cameraPos = { 0.0f, 8.0f, 5.0f };
POS cameraTarget = { 0.0f, 0.0f, 0.0f };
// multiple lights => doesnt need lightPos
// POS lightPos = { 0.0f, 3.0f, 0.0f };

Player p;

//--- load obj related variabales
objRead objReader_left_half;
objRead objReader_right_half;
objRead objReader_right_full;
objRead objReader_left_full;
objRead objReader_idle;

GLint Object_LH =	objReader_left_half.loadObj_normalize_center("models/soldier_after.obj");
GLint Object_RH =	objReader_right_half.loadObj_normalize_center("models/soldier_before.obj");
GLint Object_RF =	objReader_right_full.loadObj_normalize_center("models/soldier_2.obj");
GLint Object_LF =	objReader_left_full.loadObj_normalize_center("models/soldier_3.obj");
GLint Object_idle = objReader_idle.loadObj_normalize_center("models/soldier_idle.obj");

objRead objReader_WEAPON_handgun;
objRead objReader_WEAPON_smg;
objRead objReader_WEAPON_assault_rifle;
objRead objReader_WEAPON_sniper_rifle;
objRead objReader_WEAPON_shotgun;
objRead objReader_WEAPON_chainsaw;

GLint handgun_vertex_count =		objReader_WEAPON_handgun.loadObj_normalize_center("models/handgun.obj");
GLint smg_vertex_count =			objReader_WEAPON_smg.loadObj_normalize_center("models/smg.obj");
GLint assault_rifle_vertex_count =	objReader_WEAPON_assault_rifle.loadObj_normalize_center("models/assault_rifle.obj");
GLint sniper_rifle_vertex_count =	objReader_WEAPON_sniper_rifle.loadObj_normalize_center("models/sniper_rifle.obj");
GLint shotgun_vertex_count =		objReader_WEAPON_shotgun.loadObj_normalize_center("models/shotgun.obj");
GLint chainsaw_vertex_count =		objReader_WEAPON_chainsaw.loadObj_normalize_center("models/chainsaw.obj");


objRead objReader_street_lamp;
GLint street_lamp_vertex_count = objReader_street_lamp.loadObj_normalize_center("models/street_lamp.obj");


int player_anime = false;

int main(int argc, char** argv)
{
	// create window using freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Computer Graphics");

	//////////////////////////////////////////////////////////////////////////////////////
	//// initialize GLEW
	//////////////////////////////////////////////////////////////////////////////////////
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW OK\n";
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//// Create shader program an register the shader
	//////////////////////////////////////////////////////////////////////////////////////

	GLuint vShader[4];
	GLuint fShader[4];
	
	vShader[0] = MakeVertexShader("vertex.glsl", 0);
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	// shader Program
	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");

	InitBuffer();
	InitTexture();
	p.init();

	// callback functions
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutTimerFunc(10, TimerFunction, 1);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(PassiveMotion);


	glutMainLoop();

	return 0;
}

void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(s_program[0]);

	glm::mat4 TR = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(s_program[0], "modelTransform");

	SetProjection();
	SetCamera();
	SetLight();

	DrawFloor(TR, modelLocation);
	DrawPlayer(TR, modelLocation);
	DrawWeapon(TR, modelLocation);
	
	glutSwapBuffers();
}

void DrawPlayer(glm::mat4 TR, unsigned int modelLocation) {

	glUseProgram(s_program[0]);
	glBindVertexArray(VAO[0]);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	p.face_dir_radian = mouse_radian(g_window_middle_x, g_window_middle_y, mouse_xpos, mouse_ypos);
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, -0.05f));
	TR = glm::rotate(TR, p.face_dir_radian, glm::vec3(0.0, 1.0, 0.0));
	TR = glm::scale(TR, glm::vec3(0.5, 0.5, 0.5));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	glBindVertexArray(VAO[p.obj_num]);
	glDrawArrays(GL_TRIANGLES, 0, Object_LH);

	TR = glm::scale(TR, glm::vec3(1.0 / 0.5, 1.0 / 0.5, 1.0 / 0.5));
	TR = glm::rotate(TR, -p.face_dir_radian, glm::vec3(0.0, 1.0, 0.0));
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, 0.05f));
}

void DrawWeapon(glm::mat4 TR, unsigned int modelLocation) {
	glUseProgram(s_program[0]);
	if (p.weapon_holding == "handgun") DrawHandgun(modelLocation);
	else if (p.weapon_holding == "smg") DrawSMG(modelLocation);
	else if (p.weapon_holding == "ar") DrawAssaultRifle(modelLocation);
	else if (p.weapon_holding == "sr") DrawSniperRifle(modelLocation);
	else if (p.weapon_holding == "shotgun") DrawShotgun(modelLocation);
	else if (p.weapon_holding == "chainsaw") DrawChainsaw(modelLocation);
}

void DrawHandgun(unsigned int modelLocation) {
	// handgun
	float handgun_scale = 0.005;

	glm::mat4 TR = glm::mat4(1.0f); // -- init TR	
	glBindVertexArray(VAO_weapon[0]);
	glBindTexture(GL_TEXTURE_2D, textures[2]);

	TR = glm::translate(TR, glm::vec3(0.0, 0.0, -0.05));
	TR = glm::rotate(TR, p.face_dir_radian + glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, 0.05));

	TR = glm::translate(TR, glm::vec3(0.2, 0.24, 0.0));
	TR = glm::scale(TR, glm::vec3(handgun_scale, handgun_scale, handgun_scale));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glDrawArrays(GL_TRIANGLES, 0, handgun_vertex_count);
}

void DrawSMG(unsigned int modelLocation) {
	float smg_scale = 0.005;
	glm::mat4 TR = glm::mat4(1.0f); // -- init TR

	glBindVertexArray(VAO_weapon[1]);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, -0.05));
	TR = glm::rotate(TR, p.face_dir_radian + glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, 0.05));

	TR = glm::translate(TR, glm::vec3(0.2, 0.24, 0.0));
	TR = glm::scale(TR, glm::vec3(smg_scale, smg_scale, smg_scale));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glDrawArrays(GL_TRIANGLES, 0, smg_vertex_count);
}

void DrawAssaultRifle(unsigned int modelLocation) {
	// assault rifle
	float ar_scale = 0.005;

	glm::mat4 TR = glm::mat4(1.0f); // -- init TR
	glBindVertexArray(VAO_weapon[2]);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, -0.05));
	TR = glm::rotate(TR, p.face_dir_radian + glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, 0.05));

	TR = glm::translate(TR, glm::vec3(0.2, 0.24, 0.0));
	TR = glm::scale(TR, glm::vec3(ar_scale, ar_scale, ar_scale));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glDrawArrays(GL_TRIANGLES, 0, assault_rifle_vertex_count);
}

void DrawSniperRifle(unsigned int modelLocation) {
	float sr_scale = 0.005;
	glm::mat4 TR = glm::mat4(1.0f); // -- init TR

	glBindVertexArray(VAO_weapon[3]);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, -0.05));
	TR = glm::rotate(TR, p.face_dir_radian + glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, 0.05));

	TR = glm::translate(TR, glm::vec3(0.2, 0.24, 0.0));
	TR = glm::scale(TR, glm::vec3(sr_scale, sr_scale, sr_scale));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glDrawArrays(GL_TRIANGLES, 0, sniper_rifle_vertex_count);
}

void DrawShotgun(unsigned int modelLocation) {
	float sg_scale = 0.005;
	glm::mat4 TR = glm::mat4(1.0f); // -- init TR

	glBindVertexArray(VAO_weapon[4]);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, -0.05));
	TR = glm::rotate(TR, p.face_dir_radian + glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, 0.05));

	TR = glm::translate(TR, glm::vec3(0.2, 0.24, 0.0));
	TR = glm::scale(TR, glm::vec3(sg_scale, sg_scale, sg_scale));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glDrawArrays(GL_TRIANGLES, 0, shotgun_vertex_count);
}

void DrawChainsaw(unsigned int modelLocation) {
	float chainsaw_scale = 0.005;
	glm::mat4 TR = glm::mat4(1.0f); // -- init TR	
	
	glBindVertexArray(VAO_weapon[5]);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, -0.05));
	TR = glm::rotate(TR, p.face_dir_radian + glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, 0.05));

	TR = glm::translate(TR, glm::vec3(0.2, 0.24, 0.0));
	TR = glm::scale(TR, glm::vec3(chainsaw_scale, chainsaw_scale, chainsaw_scale));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glDrawArrays(GL_TRIANGLES, 0, chainsaw_vertex_count);
}

void DrawFloor(glm::mat4 TR, unsigned int modelLocation) {
	
	glUseProgram(s_program[0]);
	glBindVertexArray(VAO_rectangle);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	
	TR = glm::mat4(1.0f);

	TR = glm::translate(TR, glm::vec3(-p.x_move, 0.0f, p.y_move));
	TR = glm::translate(TR, glm::vec3(0.0f, -0.5f, 0.0f));
	TR = glm::scale(TR, glm::vec3(25.0f, 0.01f, 25.0f));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	TR = glm::mat4(1.0f);

	glBindVertexArray(VAO_object[0]);
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	TR = glm::translate(TR, glm::vec3(-p.x_move, -0.5f, p.y_move));
	TR = glm::scale(TR, glm::vec3(0.01f, 0.01f, 0.01f));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glDrawArrays(GL_TRIANGLES, 0, street_lamp_vertex_count);
}

void InitTexture() {
	glGenTextures(10, textures); //--- 텍스처 생성

	InsertTexture(0, "textures/soldier_texture.bmp");
	InsertTexture(1, "textures/concrete_texture.bmp");
	InsertTexture(2, "textures/gun_texture.bmp");
	InsertTexture(3, "textures/chainsaw_texture.bmp");

	InsertTexture(4, "textures/street_lamp_texture.bmp");
}

void SetProjection() {
	glm::mat4 PR = glm::mat4(1.0f);
	unsigned int projLocation = glGetUniformLocation(s_program[0], "projectionTransform");

	/* ------------------------------ 원근 투영 ------------------------------ */
	PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(PR));
	/* ---------------------------------------------------------------------------- */
}

void SetCamera() {
	glm::mat4 VR = glm::mat4(1.0f);

	glm::vec3 cameraPosVector = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z); //--- 카메라 위치
	glm::vec3 cameraTargetVector = glm::vec3(cameraTarget.x, cameraTarget.y, cameraTarget.z);
	glm::vec3 cameraDirection = glm::normalize(cameraPosVector - cameraTargetVector);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	VR = glm::lookAt(cameraPosVector, cameraDirection, cameraUp);

	unsigned int viewLocation = glGetUniformLocation(s_program[0], "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(VR));
}

void TimerFunction(int value) {
	if (player_anime) {
		switch (p.walk_num) {
		case 0: p.obj_num = 0; break;
		case 1: p.obj_num = 3; break;
		case 2: p.obj_num = 0; break;
		case 3: p.obj_num = 1; break;
		case 4: p.obj_num = 2; break;
		case 5: p.obj_num = 1; break;
		}
		if (p.walk_count % 10 == 0) {
			p.walk_num = (p.walk_num + 1) % 6;
		}
		p.walk_count++;
	}
	else {
		p.walk_count = 0;
		p.walk_num = 0;
		p.obj_num = 4;
	}

	float player_speed = p.velocity / 100;
	
	if (p.xdir == -1) {
		player_anime = true;
		p.x_move -= player_speed + 0.001;
		//cameraTarget.x = p.x_move;
	}
	else if (p.xdir == 1) {
		player_anime = true;
		p.x_move += player_speed + 0.001;
		//cameraTarget.x = p.x_move;
	}

	if (p.ydir == -1) {
		player_anime = true;
		p.y_move -= player_speed;
		//cameraTarget.z = p.y_move;
	}
	else if (p.ydir == 1) {
		player_anime = true;
		p.y_move += player_speed;
		//cameraTarget.z = p.y_move;
	}

	if (p.xdir == 0 && p.ydir == 0) {
		player_anime = false;

	}

	glutPostRedisplay();
	glutTimerFunc(10, TimerFunction, 1);
}

void Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	case 'r': case 'R':

		break;

	case 'q': case 'Q':
		exit(0);
		break;

	case 'w': case 'W': // front
		p.ydir = 1;
		break;
	case 'a': case 'A': // left
		p.xdir = -1;
		break;
	case 's': case 'S': // back
		p.ydir = -1;
		break;
	case 'd': case 'D': // right
		p.xdir = 1;
		break;

	case 'n':
		world_time = "night";
		break;
	case 'h':
		world_time = "day";
		break;
	}
	glutPostRedisplay();
}

void KeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': case 'W': // front
		if (p.ydir == 1) p.ydir = 0;
		break;
	case 'a': case 'A': // left
		if (p.xdir == -1) p.xdir = 0;
		break;
	case 's': case 'S': // back
		if (p.ydir == -1) p.ydir = 0;
		break;
	case 'd': case 'D': // right
		if (p.xdir == 1) p.xdir = 0;
		break;
	}
	glutPostRedisplay();
}

int beforeX = 0, beforeY = 0;

void Mouse(int button, int state, int x, int y) {

	if (!left_button && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		beforeX = x, beforeY = y;
		left_button = true;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		left_button = false;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		right_button = true;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		right_button = false;
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

void Motion(int x, int y) {

	mouse_xpos = x;
	mouse_ypos = y;

	// 왼쪽 마우스가 눌렸을 때
	if (left_button == true) {

		int afterX = x - beforeX;
		int afterY = y - beforeY;
		float f_afterX, f_afterY;

		beforeX = x, beforeY = y;
	}

	// 오른쪽 마우스가 눌렸을 때
	if (right_button == true) {
		;
	}
	else {
		;
	}

	glutPostRedisplay();
}

void PassiveMotion(int x, int y) {

	mouse_xpos = x;
	mouse_ypos = y;

	glutPostRedisplay();
}

void InitBuffer()
{
	glGenVertexArrays(10, VAO);
	glGenBuffers(10, VBO_position);
	glGenBuffers(10, VBO_normal);
	glGenBuffers(10, VBO_texture);

	glGenVertexArrays(1, &VAO_rectangle);
	glGenBuffers(1, &VBO_rectangle);

	glGenVertexArrays(5, VAO_object);
	glGenBuffers(5, VBO_object_position);
	glGenBuffers(5, VBO_object_normal);
	glGenBuffers(5, VBO_object_texture);

	glGenVertexArrays(7, VAO_weapon);
	glGenBuffers(7, VBO_weapon_position);
	glGenBuffers(7, VBO_weapon_normal);
	glGenBuffers(7, VBO_weapon_texture);

	// 2 triangles for quad floor
	glUseProgram(s_program[0]);
	InsertPlayerObj();
	InsertRectanglePos();
	InsertWeaponObj();
	InsertObjectObj();
}

void InsertRectanglePos() {
	glBindVertexArray(VAO_rectangle);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_rectangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Pos_rectangle), Pos_rectangle, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void InsertPlayerObjSimple(int num, objRead o) {

	glBindVertexArray(VAO[num]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position[num]);
	glBufferData(GL_ARRAY_BUFFER, o.outvertex.size() * sizeof(glm::vec3), &o.outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal[num]);
	glBufferData(GL_ARRAY_BUFFER, o.outnormal.size() * sizeof(glm::vec3), &o.outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_texture[num]);
	glBufferData(GL_ARRAY_BUFFER, o.outuv.size() * sizeof(glm::vec2), &o.outuv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);
}

void InsertWeaponObjSimple(int num, objRead o) {

	glBindVertexArray(VAO_weapon[num]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_weapon_position[num]);
	glBufferData(GL_ARRAY_BUFFER, o.outvertex.size() * sizeof(glm::vec3), &o.outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_weapon_normal[num]);
	glBufferData(GL_ARRAY_BUFFER, o.outnormal.size() * sizeof(glm::vec3), &o.outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_weapon_texture[num]);
	glBufferData(GL_ARRAY_BUFFER, o.outuv.size() * sizeof(glm::vec2), &o.outuv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);
}

void InsertObjectObjSimple(int num, objRead o) {
	glBindVertexArray(VAO_object[num]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_object_position[num]);
	glBufferData(GL_ARRAY_BUFFER, o.outvertex.size() * sizeof(glm::vec3), &o.outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_object_normal[num]);
	glBufferData(GL_ARRAY_BUFFER, o.outnormal.size() * sizeof(glm::vec3), &o.outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_object_texture[num]);
	glBufferData(GL_ARRAY_BUFFER, o.outuv.size() * sizeof(glm::vec2), &o.outuv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);
}

void InsertPlayerObj() {
	InsertPlayerObjSimple(0, objReader_left_half);
	InsertPlayerObjSimple(1, objReader_right_half);
	InsertPlayerObjSimple(2, objReader_right_full);
	InsertPlayerObjSimple(3, objReader_left_full);
	InsertPlayerObjSimple(4, objReader_idle);
}

void InsertWeaponObj() {
	InsertWeaponObjSimple(0, objReader_WEAPON_handgun);
	InsertWeaponObjSimple(1, objReader_WEAPON_smg);
	InsertWeaponObjSimple(2, objReader_WEAPON_assault_rifle);
	InsertWeaponObjSimple(3, objReader_WEAPON_sniper_rifle);
	InsertWeaponObjSimple(4, objReader_WEAPON_shotgun);
	InsertWeaponObjSimple(5, objReader_WEAPON_chainsaw);
}

void InsertObjectObj() {
	InsertObjectObjSimple(0, objReader_street_lamp);
}

void con_D_to_Ogl(int x, int y, float* ox, float* oy) {
	int w = g_window_w;
	int h = g_window_h;
	*ox = (float)((x - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0)));
	*oy = -(float)((y - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0)));
}

void con_Ogl_to_D(int* x, int* y, float ox, float oy) {
	float w = g_window_w;
	float h = g_window_h;
	*x = (int)((ox * w + w) / 2);
	*y = g_window_h - (int)((oy * h + h) / 2);
}

GLfloat mouse_radian(double x1, double y1, double x2, double y2) {
	GLfloat mouse_for_calculate_x = x2 - x1;
	GLfloat mouse_for_calculate_y = y2 - y1;
	GLfloat radian = atan2(double(mouse_for_calculate_x), double(mouse_for_calculate_y));
	return radian;
}

void InsertTexture(int textures_num, const char* path) {
	BITMAPINFO* bmp;
	glBindTexture(GL_TEXTURE_2D, textures[textures_num]); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data = LoadDIBitmap(path, &bmp); //--- 텍스처로 사용할 비트맵 이미지 로드하기
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bmp->bmiHeader.biWidth, bmp->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void SetLight() {
	glUseProgram(s_program[0]);

	glUniform1i(glGetUniformLocation(s_program[0], "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(s_program[0], "material.specular"), 1);
	glUniform1f(glGetUniformLocation(s_program[0], "material.shininess"), 32.0f);

	glUniform3f(glGetUniformLocation(s_program[0], "dirLight.direction"), 0.0f, -1.0f, -0.3f);
	if (world_time == "day") {
		glUniform3f(glGetUniformLocation(s_program[0], "dirLight.ambient"), 0.7f, 0.7f, 0.7f);
	}
	else {
		glUniform3f(glGetUniformLocation(s_program[0], "dirLight.ambient"), 0.0f, 0.0f, 0.0f);
	}
	glUniform3f(glGetUniformLocation(s_program[0], "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
	glUniform3f(glGetUniformLocation(s_program[0], "dirLight.specular"), 0.5f, 0.5f, 0.5f);

	// point light 1
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[0].position"), 100, 100, 100);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[0].ambient"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[0].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[0].quadratic"), 0.032f);

	// spotLight direction에 마우스 방향 적용  (( -> 270.0f 회전 필요.
	glm::mat4 player_radian_temp = glm::mat4(1.0f);
	player_radian_temp = glm::rotate(player_radian_temp, p.face_dir_radian + glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 dir_result = player_radian_temp * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// spotLight
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight[0].position"), 0.0f, -0.4f, -0.05f);
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight[0].direction"), dir_result[0], dir_result[1], dir_result[2]); // 점 하나 TR 에서 변환 후 대입
	if (world_time == "day") {
		glUniform3f(glGetUniformLocation(s_program[0], "spotLight[0].ambient"), 0.0f, 0.0f, 0.0f);
	}
	else {
		glUniform3f(glGetUniformLocation(s_program[0], "spotLight[0].ambient"), 0.4f, 0.4f, 0.4f);
	}
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight[0].diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight[0].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight[0].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight[0].quadratic"), 0.032f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight[0].cutOff"), glm::cos(glm::radians(25.0f)));
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight[0].outerCutOff"), glm::cos(glm::radians(30.0f)));

	// 가로등 불빛
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight[1].position"), 0.0f - p.x_move, 1.0f, 0.0f + p.y_move);
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight[1].direction"), 0.0f, -1.0f, 0.0f); // 점 하나 TR 에서 변환 후 대입

	glUniform3f(glGetUniformLocation(s_program[0], "spotLight[1].ambient"), 0.07f, 0.07f, 0.07f);
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight[1].diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight[1].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight[1].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight[1].quadratic"), 0.032f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight[1].cutOff"), glm::cos(glm::radians(60.0f)));
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight[1].outerCutOff"), glm::cos(glm::radians(80.0f)));
}