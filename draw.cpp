#include "water.h"
#include <glut.h>

void water_molecule::draw(){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor3f(1.0, 0.980392, 0.941176);
	glPointSize(radius * 2);
	glBegin(GL_POINTS);
	glVertex3f(getPosX(), getPosY(), getPosZ());
	glEnd();
	return;
}

void obj_molecule::draw(){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	if ((*this).density>1.0) glColor3f(0.254902, 0.411765, 0.882353);
	else if ((*this).density == 1.0) glColor3f(0.596078, 0.984314, 0.596078);
	else glColor3f(0.541176, 0.168627, 0.886275);
	GLUquadric *q = gluNewQuadric();
	glPushMatrix();
	glTranslatef(getPosX(), getPosY(), getPosZ());
	gluSphere(q, radius, 36, 18);
	gluDeleteQuadric(q);
	glPopMatrix();
	//glPointSize(radius * 2);
	//glBegin(GL_POINTS);
	//glVertex3f(getPosX(), getPosY(), getPosZ());
	glEnd();
	return;
}

void Water::draw(int drawObj, int showWater){
	if (showWater == 1){
		for (int i = 0; i < waterArr.size(); i++)
			waterArr[i]->draw();
	}
	if (drawObj == 1){
		for (int i = 0; i < objArr.size(); i++)
			objArr[i]->draw();
	}
	return;
}