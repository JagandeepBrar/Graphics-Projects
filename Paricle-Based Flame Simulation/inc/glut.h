/*
	Name(s): Jagandeep Brar & Ullas Gurukumar
	Student ID(s): 7739801 & 7747220
	Course: COMP4490 A01: Computer Graphics 2

	Project: Simulated Fire & Smoke Model
	File: inc/glut.h
	Details: Header for the Glut class
*/

#pragma once

#include "constants.h"
#include "libraries.h"

#include "fire.h"
#include "smoke.h"

class Glut {
	public:
		//Variables
		static GLuint program[total_objects], vao, vbo, modelView, active_axis;
		static GLfloat theta[3];
		//Prototypes
		static void adjustModelView();
		static void updateRotation(int axis);
		static void glutDisplay(void);
		static void glutDraw(void);
		static void glutInit(void);
		static void glutKeyboard(unsigned char key, int x, int y);
		static void glutMouse(int button, int state, int x, int y);
		static void glutRefresh(void);
		static void glutReshape(int width, int height);
		static void glutTimer(int unused);
		static void glutUpdate(void);
};
