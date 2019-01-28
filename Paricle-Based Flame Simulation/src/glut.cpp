/*
	Name(s): Jagandeep Brar & Ullas Gurukumar
	Student ID(s): 7739801 & 7747220
	Course: COMP4490 A01: Computer Graphics 2

	Project: Simulated Fire & Smoke Model
	File: src/glut.cpp
	Details: Holds methods required by FreeGLUT to update the worldaaaa
*/

#include "glut.h"

//Globally-accessable variables
GLuint Glut::vao, Glut::vbo, Glut::program[total_objects], Glut::modelView, Glut::active_axis = axis_x;
GLfloat Glut::theta[3] = {0.0,0.0,0.0};

/*
	Glut::glutInit
		- Initialization of the world
*/
void Glut::Glut::glutInit(void) {
	Glut::glutRefresh();
	//Initialize the VAO & VBO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Initialize the shaders/program
	program[fire_object] = Fire::initShader();
	program[smoke_object] = Smoke::initShader();
	//Enable depth testing and set the clear colour to black
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glClearColor(0,0,0,1);
}

/*
	Glut::glutRefresh()
		- Refreshes all of the data used in displaying the world
*/
void Glut::glutRefresh() {
	Fire::refreshParticles();
	Smoke::refreshParticles();
}

/*
	Glut::glutDraw()
		- Draws all of the objects
*/
void Glut::glutDraw() {
	//Draw the objects
	Fire::drawParticles();
	Smoke::drawParticles();
}

/*
	Glut::glutUpdate:
		- Updates per timer tick (per frame)
*/
void Glut::glutUpdate(void) {
	glutRefresh();
}

/*
	Glut::glutDisplay:
		- Displays the world data
*/
void Glut::glutDisplay(void) {
	//Set which buffers to be cleared
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Draw the objects, and swap the current window buffer
	glutDraw();
	glutSwapBuffers();
}

/*
	Glut::glutKeyboard:
		- Sets the keyboard control scheme
*/
void Glut::glutKeyboard(unsigned char key, int x, int y) {
	//Basic switch to handle the keypress, updates the rotation or exits depending on the key pressed
	switch (toupper(key)) {
		case 'E':
			updateRotation(axis_z);
			break;
		case 'Q':
			updateRotation(-axis_z);
			break;
		case 'W':
			updateRotation(axis_x);
			break;
		case 'S':
			updateRotation(-axis_x);
			break;
		case 'D':
			updateRotation(axis_y);
			break;
		case 'A':
			updateRotation(-axis_y);
			break;
		case 033:
			exit(EXIT_SUCCESS);
			break;
	}
}

/*
	Glut::glutMouse:
		- Sets the mouse control scheme
*/
void Glut::glutMouse(int button, int state, int x, int y) {
	//Check if a button is down, and switch between the types
	if (state == GLUT_DOWN) {
		switch (button) {
			//Left button
			case GLUT_LEFT_BUTTON: {
				return;
			}
			//Right button
			case GLUT_RIGHT_BUTTON: {
				return;
			}
			//Middle button
			case GLUT_MIDDLE_BUTTON: {
				return;
			}
		}
	}
}

/*
	Glut::glutReshape:
		- Triggered when the window is reshaped
*/
void Glut::glutReshape(int width, int height) {
	//Add re-setting the viewport size
}

/*
	Glut::glutTimer:
		- Timer function used with freeGLUT
		- Updates the world, posts a redisplay request, and sets a new timer
 */
void Glut::glutTimer(int unused) {
	glutUpdate();
	glutPostRedisplay();
	glutTimerFunc(FPS_TO_MS, glutTimer, 0);
}

/*
	Glut::adjustModelView:
		- Adjusts the modelview value to the rotation and the view distance
*/
void Glut::adjustModelView() {
	//Set the position based on the VIEW_DISTANCE
	glm::vec3 position = {0.0, 0.0, VIEW_DISTANCE};
	glm::mat4 translation, rotation, model;
	translation = glm::translate(translation, -position);
	//Calculate the rotation around all axes
	rotation = glm::rotate(rotation, glm::radians(theta[AXIS_ARRAY_LOC(axis_x)]), glm::vec3(1, 0, 0));
	rotation = glm::rotate(rotation, glm::radians(theta[AXIS_ARRAY_LOC(axis_y)]), glm::vec3(0, 1, 0));
	rotation = glm::rotate(rotation, glm::radians(theta[AXIS_ARRAY_LOC(axis_z)]), glm::vec3(0, 0, 1));
	//Calculate and set the final Uniform modelView value for the shaders
	model = translation * rotation;
	glUniformMatrix4fv(modelView, 1, GL_FALSE, glm::value_ptr(model));
}

/*
	Glut::updateRotation:
		- Updates the current theta (rotation) based on the rotation speed
*/
void Glut::updateRotation(int axis) {
	int array_loc = AXIS_ARRAY_LOC(axis);
	//Check if it is travelling in the positive or negative direction and add or subtract rotation
	if (axis > 0) {
		theta[array_loc] += ROTATION_SPEED;
	}
	else {
		theta[array_loc] -= ROTATION_SPEED;
	}
	//Check if has passed a full circle and cause a wraparound
	if (theta[array_loc] >= 360) {
		theta[array_loc] = 0;
	} else if (theta[array_loc] <= 0) {
		theta[array_loc] = 360;
	}
}
