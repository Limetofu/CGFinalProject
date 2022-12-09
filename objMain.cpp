#define _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include "objRead.h"
#include "player.h"
#include "texture.h"

#define TRI_COUNT 1000

using namespace std;

GLuint g_window_w = 1280;
GLuint g_window_h = 720;

int g_window_middle_x = g_window_w / 2;
int g_window_middle_y = g_window_h / 2;

GLuint VAO[10];
GLuint VBO_position[10];
GLuint VBO_normal[10];
GLuint VBO_texture[10];

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

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void InitBuffer();
void InsertObj();
void InitLight(char col);
void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void PassiveMotion(int x, int y);

GLfloat mouse_radian(double x1, double y1, double x2, double y2);
void DrawPlayer(glm::mat4 TR, unsigned int modelLocation);
void DrawFloor(glm::mat4 TR, unsigned int modelLocation);

void SetCamera();
void SetProjection();
void SetMaterial(Material m);
void InitTexture();

Material DesertMaterial = {
	{ 1.0f, 0.5f, 0.31f },
	{ 0.7f, 0.42f, 0.26f },
	{ 0.5f, 0.5f, 0.5f },
	{32.0f}
};

GLfloat cameraPosZ = 10.0f;
GLfloat cameraPosX = 0.0f;
GLfloat cameraPosY = 5.0f;

GLfloat light_radian = 3.0f;
GLfloat lightX = 0.0f, lightY = 0.0f, lightZ = 2.0f;
int light_rotate = false;
GLfloat light_degree = 0.0f;

int light_on = true;
int light_color = 0;

int obj_num = 0;
int walk_num = 0;

//--- load obj related variabales
objRead objReader_left_half;
objRead objReader_right_half;
objRead objReader_right_full;
objRead objReader_left_full;
objRead objReader_idle;

GLint Object_LH = objReader_left_half.loadObj_normalize_center("soldier_after.obj");
GLint Object_RH = objReader_right_half.loadObj_normalize_center("soldier_before.obj");
GLint Object_RF = objReader_right_full.loadObj_normalize_center("soldier_2.obj");
GLint Object_LF = objReader_left_full.loadObj_normalize_center("soldier_3.obj");
GLint Object_idle = objReader_idle.loadObj_normalize_center("soldier_idle.obj");

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

	/* ------------------------------ Initilize Camera, Proj ------------------------------ */
	SetProjection();
	SetCamera();
	/* ------------------------------------------------------------------------------------ */
	SetMaterial(DesertMaterial);


	DrawFloor(TR, modelLocation);
	DrawPlayer(TR, modelLocation);
	
	glutSwapBuffers();
}




void SetMaterial(Material m) {
	glUseProgram(s_program[0]);

	glUniform3f(glGetUniformLocation(s_program[0], "material.ambient"), m.ambient[0], m.ambient[1], m.ambient[2]);
	glUniform3f(glGetUniformLocation(s_program[0], "material.specular"), m.specular[0], m.specular[1], m.specular[2]);
	glUniform3f(glGetUniformLocation(s_program[0], "material.diffuse"), m.diffuse[0], m.diffuse[1], m.diffuse[2]);
	glUniform1f(glGetUniformLocation(s_program[0], "material.shininess"), m.shininess);
}

void DrawPlayer(glm::mat4 TR, unsigned int modelLocation) {
	InitLight('g');

	glUseProgram(s_program[0]);
	glBindVertexArray(VAO[0]);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	GLfloat mouse_pos_radian = mouse_radian(g_window_middle_x, g_window_middle_y, mouse_xpos, mouse_ypos);
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, -0.05f));
	TR = glm::rotate(TR, mouse_pos_radian, glm::vec3(0.0, 1.0, 0.0));
	TR = glm::scale(TR, glm::vec3(0.5, 0.5, 0.5));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	glBindVertexArray(VAO[obj_num]);
	glDrawArrays(GL_TRIANGLES, 0, Object_LH);

	TR = glm::scale(TR, glm::vec3(1.0 / 0.5, 1.0 / 0.5, 1.0 / 0.5));
	TR = glm::rotate(TR, -mouse_pos_radian, glm::vec3(0.0, 1.0, 0.0));
	TR = glm::translate(TR, glm::vec3(0.0, 0.0, 0.05f));
}

void DrawFloor(glm::mat4 TR, unsigned int modelLocation) {

}

void InitTexture() {
	BITMAPINFO* bmp;
	glGenTextures(1, &textures[0]); //--- 텍스처 생성
	glBindTexture(GL_TEXTURE_2D, textures[0]); //--- 텍스처 바인딩
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data = LoadDIBitmap("soldier_texture.bmp", &bmp); //--- 텍스처로 사용할 비트맵 이미지 로드하기
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bmp->bmiHeader.biWidth, bmp->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //---텍스처 이미지 정의
	printf("%d, %d\n", bmp->bmiHeader.biWidth, bmp->bmiHeader.biHeight);
}

void InitLight(char col) {
	lightX = cos(glm::radians(light_degree + 90.0f)) * light_radian;
	lightZ = sin(glm::radians(light_degree + 90.0f)) * light_radian;

	glUseProgram(s_program[0]);

	unsigned int lightPosLocation = glGetUniformLocation(s_program[0], "lightPos");
	glUniform3f(lightPosLocation, lightX, lightY, lightZ);

	unsigned int lightColorLocation = glGetUniformLocation(s_program[0], "lightColor");
	if (light_color == 0) glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	else if (light_color == 1) glUniform3f(lightColorLocation, 0.8, 0.15, 0.15);
	else if (light_color == 2) glUniform3f(lightColorLocation, 0.15, 0.8, 0.15);
	else if (light_color == 3) glUniform3f(lightColorLocation, 0.15, 0.15, 0.8);

	unsigned int objColorLocation = glGetUniformLocation(s_program[0], "objectColor");
	if (col == 'r') glUniform3f(objColorLocation, 0.95, 0.13, 0.12);
	else if (col == 'g') glUniform3f(objColorLocation, 0.12, 0.96, 0.13);
	else if (col == 'b') glUniform3f(objColorLocation, 0.09, 0.11, 0.94);

	unsigned int viewPosLocation = glGetUniformLocation(s_program[0], "viewPos");
	glUniform3f(viewPosLocation, cameraPosX, cameraPosY, cameraPosZ);
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

	glm::vec3 cameraPos = glm::vec3(cameraPosX, cameraPosY, cameraPosZ); //--- 카메라 위치
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	unsigned int viewLocation = glGetUniformLocation(s_program[0], "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(VR));
}

void TimerFunction(int value) {
	// lightZ = 2.0f
	if (light_rotate) {
		lightX = cos(glm::radians(light_degree + 90.0f)) * light_radian;
		lightZ = sin(glm::radians(light_degree + 90.0f)) * light_radian;

		light_degree += 1.0f;
		if (light_degree >= 360.0f) light_degree = 0.0f;
	}

	if (player_anime) {
		switch (walk_num) {
		case 0: obj_num = 0; break;
		case 1: obj_num = 3; break;
		case 2: obj_num = 0; break;
		case 3: obj_num = 1; break;
		case 4: obj_num = 2; break;
		case 5: obj_num = 1; break;
		}

		walk_num = (walk_num + 1) % 6;
	}
	else {
		walk_num = 0;
		obj_num = 4;
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
		if (light_rotate) light_rotate = false;
		else light_rotate = true;
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

	// 2 triangles for quad floor
	glUseProgram(s_program[0]);
	InsertObj();
}

void InsertObj() {
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