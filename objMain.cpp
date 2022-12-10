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

GLfloat test_scale_value = 0.02f;

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


glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

// camera basic pos = 0, 8, 5
POS cameraPos = { 0.0f, 8.0f, 5.0f };
// multiple lights => doesnt need lightPos
// POS lightPos = { 0.0f, 3.0f, 0.0f };

Player p;

int light_on = true;
int light_color = 0;

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
// objRead objReader_WEAPON_flame_thrower;
objRead objReader_WEAPON_chainsaw;

GLint handgun_vertex_count =		objReader_WEAPON_handgun.loadObj_normalize_center("models/handgun.obj");
GLint smg_vertex_count =			objReader_WEAPON_smg.loadObj_normalize_center("models/smg.obj");
GLint assault_rifle_vertex_count =	objReader_WEAPON_assault_rifle.loadObj_normalize_center("models/assault_rifle.obj");
GLint sniper_rifle_vertex_count =	objReader_WEAPON_sniper_rifle.loadObj_normalize_center("models/sniper_rifle.obj");
GLint shotgun_vertex_count =		objReader_WEAPON_shotgun.loadObj_normalize_center("models/shotgun.obj");
// GLint flame_thrower_vertex_count =	objReader_WEAPON_flame_thrower.loadObj_normalize_center("models/flame_thrower.obj");
GLint chainsaw_vertex_count =			objReader_WEAPON_chainsaw.loadObj_normalize_center("models/chainsaw.obj");



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
	glutTimerFunc(100, TimerFunction, 1);
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

	if (print_solid) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

void SetLight() {
	glUseProgram(s_program[0]);

	glUniform1i(glGetUniformLocation(s_program[0], "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(s_program[0], "material.specular"), 1);
	glUniform1f(glGetUniformLocation(s_program[0], "material.shininess"), 32.0f);

	glUniform3f(glGetUniformLocation(s_program[0], "dirLight.direction"), -1.0f, -1.0f, -0.3f);
	glUniform3f(glGetUniformLocation(s_program[0], "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(s_program[0], "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
	glUniform3f(glGetUniformLocation(s_program[0], "dirLight.specular"), 0.5f, 0.5f, 0.5f);

	// point light 1
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[0].position"), 0.7f, 0.2f, 2.0f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[0].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[0].quadratic"), 0.032f);
	// point light 2
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[1].position"), 2.3f, -3.3f, -4.0f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[1].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[1].quadratic"), 0.032f);
	// point light 3
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[2].position"), -4.0f, 2.0f, -12.0f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(s_program[0], "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[2].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(s_program[0], "pointLights[2].quadratic"), 0.032f);

	// spotLight direction에 마우스 방향 적용  (( -> 270.0f 회전 필요.
	glm::mat4 player_radian_temp = glm::mat4(1.0f);
	player_radian_temp = glm::rotate(player_radian_temp, p.face_dir_radian + glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 dir_result = player_radian_temp * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// spotLight
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight.position"), 0.0f, -0.4f, -0.05f);
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight.direction"), dir_result[0], dir_result[1], dir_result[2]); // 점 하나 TR 에서 변환 후 대입
	
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(s_program[0], "spotLight.specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight.linear"), 0.09f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight.quadratic"), 0.032f);
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight.cutOff"), glm::cos(glm::radians(27.0f)));
	glUniform1f(glGetUniformLocation(s_program[0], "spotLight.outerCutOff"), glm::cos(glm::radians(32.0f)));

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

	/* ---- player rotate point = {0.0, 0.0, -0.05f} ----*/
	/* ---- player rotate point = {0.0, 0.0, -0.05f} ----*/
	/* ---- player rotate point = {0.0, 0.0, -0.05f} ----*/
	float ar_scale = 0.005;
	glUseProgram(s_program[0]);

	// handgun
	glBindVertexArray(VAO_weapon[0]);
	glBindTexture(GL_TEXTURE_2D, textures[2]);

	TR = glm::translate(TR, glm::vec3(2.0, 0.4, 0.5f));
	TR = glm::scale(TR, glm::vec3(ar_scale, ar_scale, ar_scale));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	//glDrawArrays(GL_TRIANGLES, 0, handgun_vertex_count);
	
	TR = glm::mat4(1.0f); // -- init TR

	// assault rifle
	glBindVertexArray(VAO_weapon[2]);
	glBindTexture(GL_TEXTURE_2D, textures[2]);

	TR = glm::translate(TR, glm::vec3(0.0, 0.0, -0.05));
	TR = glm::rotate(TR, p.face_dir_radian + glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, 0.05));
	
	TR = glm::translate(TR, glm::vec3(0.2, 0.24, 0.0));
	TR = glm::scale(TR, glm::vec3(ar_scale, ar_scale, ar_scale));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glDrawArrays(GL_TRIANGLES, 0, assault_rifle_vertex_count);


	// chainsaw
	glBindVertexArray(VAO_weapon[5]);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	TR = glm::translate(TR, glm::vec3(-2.0, 0.4, 0.5f));
	TR = glm::scale(TR, glm::vec3(test_scale_value, test_scale_value, test_scale_value));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glDrawArrays(GL_TRIANGLES, 0, chainsaw_vertex_count);
}

void DrawFloor(glm::mat4 TR, unsigned int modelLocation) {
	
	glUseProgram(s_program[0]);
	glBindVertexArray(VAO_rectangle);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	
	TR = glm::translate(TR, glm::vec3(0.0f, -0.5f, 0.0f));
	TR = glm::scale(TR, glm::vec3(20.0f, 0.01f, 20.0f));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void InitTexture() {
	glGenTextures(10, textures); //--- 텍스처 생성

	InsertTexture(0, "textures/soldier_texture.bmp");
	InsertTexture(1, "textures/concrete_texture.bmp");
	InsertTexture(2, "textures/gun_texture.bmp");
	InsertTexture(3, "textures/chainsaw_texture.bmp");
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
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(cameraPosVector - cameraTarget);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	VR = glm::lookAt(cameraPosVector, cameraDirection, cameraUp);

	unsigned int viewLocation = glGetUniformLocation(s_program[0], "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(VR));
}

void TimerFunction(int value) {
	/*if (light_rotate) {
		lightPos.x = cos(glm::radians(light_degree + 90.0f)) * light_radian;
		lightPos.z = sin(glm::radians(light_degree + 90.0f)) * light_radian;

		light_degree += 1.0f;
		if (light_degree >= 360.0f) light_degree = 0.0f;
	}*/

	if (player_anime) {
		switch (p.walk_num) {
		case 0: p.obj_num = 0; break;
		case 1: p.obj_num = 3; break;
		case 2: p.obj_num = 0; break;
		case 3: p.obj_num = 1; break;
		case 4: p.obj_num = 2; break;
		case 5: p.obj_num = 1; break;
		}

		p.walk_num = (p.walk_num + 1) % 6;
	}
	else {
		p.walk_num = 0;
		p.obj_num = 4;
	}

	glutPostRedisplay();
	InitBuffer();
	glutTimerFunc(100, TimerFunction, 1);
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
	case 'c': case 'C':
		light_color = (light_color + 1) % 4;
		break;

	case 'r': case 'R':

		break;

	case 'q': case 'Q':
		exit(0);
		break;

	case 'w': case 'W': // front
		player_anime = true;
		break;
	case 'a': case 'A': // left
		player_anime = true;
		break;
	case 's': case 'S': // back
		player_anime = true;
		break;
	case 'd': case 'D': // right
		player_anime = true;
		break;

	case 'l':
		if (player_anime) player_anime = false;
		else player_anime = true;
		break;
	}
	glutPostRedisplay();
}

void KeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': case 'W': // front
		player_anime = false;
		break;
	case 'a': case 'A': // left
		player_anime = false;
		break;
	case 's': case 'S': // back
		player_anime = false;
		break;
	case 'd': case 'D': // right
		player_anime = false;
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

	glGenVertexArrays(7, VAO_weapon);
	glGenBuffers(7, VBO_weapon_position);
	glGenBuffers(7, VBO_weapon_normal);
	glGenBuffers(7, VBO_weapon_texture);

	// 2 triangles for quad floor
	glUseProgram(s_program[0]);
	InsertPlayerObj();
	InsertRectanglePos();
	InsertWeaponObj();
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

void InsertPlayerObj() {
	// left half
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_position[0]);
	glBufferData(GL_ARRAY_BUFFER, objReader_left_half.outvertex.size() * sizeof(glm::vec3), &objReader_left_half.outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal[0]);
	glBufferData(GL_ARRAY_BUFFER, objReader_left_half.outnormal.size() * sizeof(glm::vec3), &objReader_left_half.outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_texture[0]);
	glBufferData(GL_ARRAY_BUFFER, objReader_left_half.outuv.size() * sizeof(glm::vec2), &objReader_left_half.outuv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	// right half
	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_position[1]);
	glBufferData(GL_ARRAY_BUFFER, objReader_right_half.outvertex.size() * sizeof(glm::vec3), &objReader_right_half.outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal[1]);
	glBufferData(GL_ARRAY_BUFFER, objReader_right_half.outnormal.size() * sizeof(glm::vec3), &objReader_right_half.outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_texture[1]);
	glBufferData(GL_ARRAY_BUFFER, objReader_right_half.outuv.size() * sizeof(glm::vec2), &objReader_right_half.outuv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	// right full
	glBindVertexArray(VAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_position[2]);
	glBufferData(GL_ARRAY_BUFFER, objReader_right_full.outvertex.size() * sizeof(glm::vec3), &objReader_right_full.outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal[2]);
	glBufferData(GL_ARRAY_BUFFER, objReader_right_full.outnormal.size() * sizeof(glm::vec3), &objReader_right_full.outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_texture[2]);
	glBufferData(GL_ARRAY_BUFFER, objReader_right_full.outuv.size() * sizeof(glm::vec2), &objReader_right_full.outuv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	// left full
	glBindVertexArray(VAO[3]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_position[3]);
	glBufferData(GL_ARRAY_BUFFER, objReader_left_full.outvertex.size() * sizeof(glm::vec3), &objReader_left_full.outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal[3]);
	glBufferData(GL_ARRAY_BUFFER, objReader_left_full.outnormal.size() * sizeof(glm::vec3), &objReader_left_full.outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_texture[3]);
	glBufferData(GL_ARRAY_BUFFER, objReader_left_full.outuv.size() * sizeof(glm::vec2), &objReader_left_full.outuv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	// idle
	glBindVertexArray(VAO[4]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_position[4]);
	glBufferData(GL_ARRAY_BUFFER, objReader_idle.outvertex.size() * sizeof(glm::vec3), &objReader_idle.outvertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal[4]);
	glBufferData(GL_ARRAY_BUFFER, objReader_idle.outnormal.size() * sizeof(glm::vec3), &objReader_idle.outnormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_texture[4]);
	glBufferData(GL_ARRAY_BUFFER, objReader_idle.outuv.size() * sizeof(glm::vec2), &objReader_idle.outuv[0], GL_STATIC_DRAW);
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

void InsertWeaponObj() {
	InsertWeaponObjSimple(0, objReader_WEAPON_handgun);
	InsertWeaponObjSimple(1, objReader_WEAPON_smg);
	InsertWeaponObjSimple(2, objReader_WEAPON_assault_rifle);
	InsertWeaponObjSimple(3, objReader_WEAPON_sniper_rifle);
	InsertWeaponObjSimple(4, objReader_WEAPON_handgun);
	InsertWeaponObjSimple(5, objReader_WEAPON_chainsaw);
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