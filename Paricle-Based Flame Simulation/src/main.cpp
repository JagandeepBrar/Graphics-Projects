/*
	Name(s): Jagandeep Brar & Ullas Gurukumar
	Student ID(s): 7739801 & 7747220
	Course: COMP4490 A01: Computer Graphics 2

	Project: Simulated Fire & Smoke Model
	File: src/main.cpp
	Details: Holds the main method that initializes/starts GLEW, FreeGLUT, and OpenGL
*/

#include "constants.h"
#include "glut.h"
#include "libraries.h"
#include "shaders.h"

int main(int argc, char **argv) {
	//Initializing the RNG
	srand((unsigned int)time(NULL));
	//Initializing GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(OPENGL_MAJOR_VER,OPENGL_MINOR_VER);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutCreateWindow(WINDOW_TITLE);
	//Initializing GLEW
	glewInit();
	//Setting GLUT Functions
	glutDisplayFunc(Glut::glutDisplay);
	glutKeyboardFunc(Glut::glutKeyboard);
	glutMouseFunc(Glut::glutMouse);
	glutReshapeFunc(Glut::glutReshape);
	glutTimerFunc(FPS_TO_MS, Glut::glutTimer, 0);
	//Run the GLUT main processing loop
	Glut::glutInit();
	glutMainLoop();
	return 0;
}