#include <Windows.h>
#include <iostream>
#include <glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "water.h"

using namespace std;
int resolution;	//3차원 공간에 한 변에 존재할 grid의 개수
int size; // 공간의 크기 
int rotateX = 0;
int rotateY = 0;
int rotateZ = 0;
int angleX = 0;
int angleY = 0;
int angleZ = 0;
int putObj = 0;
int drawObj = 0;
int oil = 0;
int heavy_ink = 1;
int light_ink = 0;
double obj_density = 1.3;
double temperature = 25;
int tempUp = 0;
int tempDown = 0;
int showWater = 1;
int hideWater = 0;
Water *water;
void data_inil(){
	resolution =25; 
	size =50;
	water = new Water();
	water->init(resolution, size);
}
void update(){
	double dt = 0.002;
	if (tempUp == 1){
		temperature++;
		tempUp = 0;
	}
	if (tempDown == 1){
		temperature--;
		tempDown = 0;
	}
	if (temperature < 0) temperature = 0;
	if (heavy_ink == 1) obj_density = 1.5;
	if (light_ink == 1) obj_density = 1.0;
	if (oil == 1) obj_density = 0.5;
	water->integrate(dt, temperature, obj_density, putObj);
}
void Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(50.0f, 50.0f,50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH);
	// 여기에 코드를 작성하십시오.
	glBegin(GL_LINES);
	glColor3f(1.0, 1.0, 0);//yellow
	glVertex3f(-300, 0, 0);
	glVertex3f(300, 0, 0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.5, 1.0, 1.0);//cyan
	glVertex3f(0, -300, 0);
	glVertex3f(0, 300, 0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0, 0.5, 1.0);//babyblue
	glVertex3f(0, 0, -300);
	glVertex3f(0, 0, 300);
	glEnd();
	//draw molecules
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-size / 2, -size / 2, size/2);
	if (rotateX ==1) glRotatef(angleX, 1, 0, 0);
	if (rotateY == 1) glRotatef(angleY, 0, 1, 0);
	if (rotateZ == 1) glRotatef(angleZ, 0, 0, 1);
	if (showWater==1) water->draw(drawObj, 1);
	else if (hideWater == 1) water->draw(drawObj, 0);
	glPopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, w / h, 1.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Mouse(int mouse_event, int state, int x, int y)
{
	switch (mouse_event)
	{
	case GLUT_LEFT_BUTTON:
		putObj = 1;
		break;
	case GLUT_MIDDLE_BUTTON:

		break;
	case GLUT_RIGHT_BUTTON:
		drawObj = 1;
		break;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}


void Motion(int x, int y)
{

	glutPostRedisplay();
}


void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
	case VK_ESCAPE:
		exit(0);
		break;
	case '1':
		oil = 0;
		heavy_ink = 1;
		light_ink = 0;
		printf("put heavy ink into the water\n");
		break;
	case '2':
		oil = 1;
		heavy_ink = 0;
		light_ink = 0;
		printf("put oil into the water\n");
		break;
	case '3':
		oil = 0;
		heavy_ink = 0;
		light_ink = 1;
		printf("put light ink into the water\n");
		break;

	case 'a':
		tempUp = 1;
		tempDown = 0;
		printf("temperature: %f\n", temperature+1.0);
		break;
	case 's':
		tempUp = 0;
		tempDown = 1;
		printf("temperature: %f\n", temperature-1.0);
		break;
	case 'z':
		showWater = 1;
		hideWater = 0;
		printf("show water molecules\n");
		break;
	case 'x':
		showWater = 0;
		hideWater = 1;
		printf("hide water molecules\n");
		break;

	}
	glutPostRedisplay();
}

void SpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		break;
	case GLUT_KEY_LEFT:
		rotateY = 1;
		angleY++;
		break;
	case GLUT_KEY_RIGHT:
		rotateY = 1;
		angleY--;
		break;
	case GLUT_KEY_UP:
		rotateZ = 1;
		angleZ++;
		break;
	case GLUT_KEY_DOWN:
		rotateZ = 1;
		angleZ--;
		break;

	}
	glutPostRedisplay();
}

void Idle(void)
{

	glutPostRedisplay();
}
int main(int argc, char ** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(1000, 1000);
	//Window 제목은 학번과 학생 이름으로 작성하십시오.
	glutCreateWindow("2016 CG Final Assignment THE BROWN MOVEMENT");
	printf("========================INFORMATION========================\n\n");
	printf("\t press a: temperature down\n");
	printf("\t press s: temperature down\n\n");
	printf("\t press 1: put heavy ink\n");
	printf("\t press 2: put oil\n");
	printf("\t press 3: put light ink\n\n");
	printf("\t press z: SHOW water molecules\n");
	printf("\t press x: HIDE water molecules\n\n");
	printf("\t clik right mouse button: initialize object\n\n");
	printf("\t clik left mouse button: put obj into water\n\n");

	printf("************************CAUTION************************\n\n");
	printf("\t MUST CLICK in oder of RIGHT --> LEFT \n\n");
	printf("*******************************************************\n\n");
	printf("\t keyboard arrow button to rotate\n");
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	data_inil();

	glutIdleFunc(update);
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);

	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKey);
	glutMainLoop();

	return 0;
}

