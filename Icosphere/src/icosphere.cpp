/*
 * Name: Jagandeep Brar
 * Student ID: 7739801
 * Course: COMP4490 A01
 * Assignment: A02
 * Question: Q01
 * File: icosphere.cpp
 * 
 * Using the supplied OpenGL code (sepcifically example6.cpp):
 * Create an icosphere with half being perturbed in the vertex and fragment shaders
 */

#include "common.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//My defines for the aount of subdivides
#define ICOSPHERE_SUBDIVISIONS 4
#define VIEW_DISTANCE 3
//Icosphere supplied defines
#define X .525731112119133606
#define Z .850650808352039932

//Global Constants
const char *WINDOW_TITLE = "Icosphere";
const double FRAME_RATE_MS = 1000.0/60.0;

//Global Variables
static enum {
	axisX,
	axisY,
	axisZ,
	numAxes
};
int Axis = axisX;
GLfloat Theta[numAxes] = { 0.0, 0.0, 0.0 };

//Holds the model, projection, and border Uniform values
GLuint  ModelView, Projection, Border;

//Holds the indices and vertices
std::vector<glm::vec4> vertices;
std::vector<GLuint> indices;

/*
 * THIS IS FROM THE ORIGINAL ICOSPHERE.CPP
*/

//Prototypes
void icosphere(int sub, std::vector<glm::vec4> &vertices, std::vector<GLuint> &indices);

static GLfloat vdata[12][3] = {
	{ -X, 0.0, Z },{ X, 0.0, Z },{ -X, 0.0, -Z },{ X, 0.0, -Z },
	{ 0.0, Z, X },{ 0.0, Z, -X },{ 0.0, -Z, X },{ 0.0, -Z, -X },
	{ Z, X, 0.0 },{ -Z, X, 0.0 },{ Z, -X, 0.0 },{ -Z, -X, 0.0 }
};

static GLuint tindices[20][3] = {
	{ 0,4,1 },{ 0,9,4 },{ 9,5,4 },{ 4,5,8 },{ 4,8,1 },
	{ 8,10,1 },{ 8,3,10 },{ 5,3,8 },{ 5,2,3 },{ 2,7,3 },
	{ 7,10,3 },{ 7,6,10 },{ 7,11,6 },{ 11,0,6 },{ 0,1,6 },
	{ 6,1,10 },{ 9,0,11 },{ 9,11,2 },{ 9,2,5 },{ 7,2,11 }
};

void icosphere(int sub, std::vector<glm::vec4> &vertices, std::vector<GLuint> &indices) {
	for (GLfloat *v : vdata) {
		vertices.push_back(glm::vec4(v[0], v[1], v[2], 1));
	}
	indices.assign((GLuint *)tindices, (GLuint *)tindices + (sizeof(tindices) / sizeof(GLuint)));

	// use a regular subdivision (each tri into 4 equally-sized)
	for (int s = 0; s < sub; s++) {
		int isize = indices.size();
		for (int tri = 0; tri < isize; tri += 3) {
			int i0 = indices[tri];
			int i1 = indices[tri + 1];
			int i2 = indices[tri + 2];

			// create midpoints and "push" them out to the unit sphere
			// (but exclude the homogeneous coordinate)
			glm::vec3 midpoint0(glm::normalize(glm::vec3(vertices[i0] + vertices[i1]) * 0.5f));
			glm::vec3 midpoint1(glm::normalize(glm::vec3(vertices[i1] + vertices[i2]) * 0.5f));
			glm::vec3 midpoint2(glm::normalize(glm::vec3(vertices[i2] + vertices[i0]) * 0.5f));

			// add the midpoints to the vertices list
			int m0 = vertices.size();
			int m1 = m0 + 1;
			int m2 = m0 + 2;
			vertices.push_back(glm::vec4(midpoint0, 1));
			vertices.push_back(glm::vec4(midpoint1, 1));
			vertices.push_back(glm::vec4(midpoint2, 1));

			// now the four triangles
			indices[tri + 1] = m0;
			indices[tri + 2] = m2;
			indices.push_back(m0);
			indices.push_back(i1);
			indices.push_back(m1);
			indices.push_back(m0);
			indices.push_back(m1);
			indices.push_back(m2);
			indices.push_back(m2);
			indices.push_back(m1);
			indices.push_back(i2);
		}
	}
}

/*
 * END OF THE ORIGINAL ICOSPHERE.CPP
 */

void init() {
	//Calculates and stores the icosphere vertices and indices
	icosphere(ICOSPHERE_SUBDIVISIONS, vertices, indices);
	GLuint vao, vbo;
	//Initialize the VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//Create and bind the VBO (vertices)
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4), &vertices[0], GL_STATIC_DRAW);
	//Create and bind the VBO (indices)
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	//Loads my basic shader into program
	GLuint program = InitShader("../src/vshader.glsl", "../src/fshader.glsl");
	glUseProgram(program);
	//Load the vPosition for the vshader
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	//Load the Uniform values for the program
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	Border = glGetUniformLocation( program, "Border" );
	//Enable depth testing and set the clear colour to white
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void processTriangles() {
	//Transform and rotate the sphere (triangles)
	//Make the Z-distance to VIEW_DISTANCE
	const glm::vec3 viewer_pos(0.0, 0.0, VIEW_DISTANCE);
	glm::mat4 trans, rot, model_view;
	trans = glm::translate(trans, -viewer_pos);
	rot = glm::rotate(rot, glm::radians(Theta[axisX]), glm::vec3(1, 0, 0));
	rot = glm::rotate(rot, glm::radians(Theta[axisY]), glm::vec3(0, 1, 0));
	rot = glm::rotate(rot, glm::radians(Theta[axisZ]), glm::vec3(0, 0, 1));
	model_view = trans * rot;
	//Generate the model-view matrix
	glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));
	//Set the "Border" switch to 0, so it renders triangles and colours
	glUniform1i(Border, 0);
	//For each set of indices, render a filled triangle
	for (int i = 0; i < indices.size(); i += 3) {
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)));
	}
}

void processBorders() {
	//Transform and rotate the sphere (border)
	//Make the Z-distance slightly closer to make the black borders pop out
	const glm::vec3 viewer_pos(0.0, 0.0, VIEW_DISTANCE-0.0025);
	glm::mat4 trans, rot, model_view;
	trans = glm::translate(trans, -viewer_pos);
	rot = glm::rotate(rot, glm::radians(Theta[axisX]), glm::vec3(1, 0, 0));
	rot = glm::rotate(rot, glm::radians(Theta[axisY]), glm::vec3(0, 1, 0));
	rot = glm::rotate(rot, glm::radians(Theta[axisZ]), glm::vec3(0, 0, 1));
	model_view = trans * rot;
	//Generate the model-view matrix
	glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));
	//Set the "Border" switch to 1, so it renders just the edges in black
	glUniform1i(Border, 1);
	//For each set of indices, render a lined loop (border)
	for (int i = 0; i < indices.size(); i += 3) {
		glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)));
	}
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Seperated into methods for rendering triangles and borders for easier reading
	processTriangles();
	processBorders();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033: case 'q': case 'Q':
		{
			exit(EXIT_SUCCESS);
			break;
		}
	}
}

void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		switch (button) {
			case GLUT_LEFT_BUTTON:    Axis = axisX;  break;
			case GLUT_MIDDLE_BUTTON:  Axis = axisY;  break;
			case GLUT_RIGHT_BUTTON:   Axis = axisZ;  break;
		}
	}
}

void update(void) {
	Theta[Axis] += 0.5;
	if (Theta[Axis] > 360.0) {
		Theta[Axis] -= 360.0;
	}
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	GLfloat aspect = GLfloat(width) / height;
	glm::mat4  projection = glm::perspective(glm::radians(45.0f), aspect, 0.5f, 3.0f);
	glUniformMatrix4fv(Projection, 1, GL_FALSE, glm::value_ptr(projection));
}
