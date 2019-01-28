/*
 * Name: Jagandeep Brar
 * Student ID: 7739801
 * Course: COMP4490 A01
 * Assignment: A02
 * Question: Q02
 * File: teapot.cpp
 * 
 * Generates a utah teapot using bezier's curve
 */

#include "common.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TEAPOT_PATCHES 32
#define TEAPOT_VERTICES 306
#define GRID_SIZE 8
#define SCALE_FACTOR 5
#define VIEW_DISTANCE 0.5

const char *WINDOW_TITLE = "Utah Teapot";
const double FRAME_RATE_MS = 1000.0 / 60.0;

GLuint ModelView;

//Holds the indices and vertices, and current working control points
std::vector<glm::vec4> vertices;
std::vector<GLuint> indices;
glm::vec3 control_points[4][4];

enum {
	axisX = 0,
	axisY = 1,
	axisZ = 2
};
int Axis = axisX;
GLfloat Theta[3] = { 0.0, 0.0, 0.0 };

glm::vec3 teapot_vertices[TEAPOT_VERTICES] = {
	glm::vec3(1.4,0.0,2.4),glm::vec3(1.4,-0.784,2.4),glm::vec3(0.784,-1.4,2.4),glm::vec3(0.0,-1.4,2.4),
	glm::vec3(1.3375,0.0,2.53125),glm::vec3(1.3375,-0.749,2.53125),glm::vec3(0.749,-1.3375,2.53125),glm::vec3(0.0,-1.3375,2.53125),
	glm::vec3(1.4375,0.0,2.53125),glm::vec3(1.4375,-0.805,2.53125),glm::vec3(0.805,-1.4375,2.53125),glm::vec3(0.0,-1.4375,2.53125),
	glm::vec3(1.5,0.0,2.4),glm::vec3(1.5,-0.84,2.4),glm::vec3(0.84,-1.5,2.4),glm::vec3(0.0,-1.5,2.4),
	glm::vec3(-0.784,-1.4,2.4),glm::vec3(-1.4,-0.784,2.4),glm::vec3(-1.4,0.0,2.4),glm::vec3(-0.749,-1.3375,2.53125),
	glm::vec3(-1.3375,-0.749,2.53125),glm::vec3(-1.3375,0.0,2.53125),glm::vec3(-0.805,-1.4375,2.53125),glm::vec3(-1.4375,-0.805,2.53125),
	glm::vec3(-1.4375,0.0,2.53125),glm::vec3(-0.84,-1.5,2.4),glm::vec3(-1.5,-0.84,2.4),glm::vec3(-1.5,0.0,2.4),
	glm::vec3(-1.4,0.784,2.4),glm::vec3(-0.784,1.4,2.4),glm::vec3(0.0,1.4,2.4),glm::vec3(-1.3375,0.749,2.53125),
	glm::vec3(-0.749,1.3375,2.53125),glm::vec3(0.0,1.3375,2.53125),glm::vec3(-1.4375,0.805,2.53125),glm::vec3(-0.805,1.4375,2.53125),
	glm::vec3(0.0,1.4375,2.53125),glm::vec3(-1.5,0.84,2.4),glm::vec3(-0.84,1.5,2.4),glm::vec3(0.0,1.5,2.4),
	glm::vec3(0.784,1.4,2.4),glm::vec3(1.4,0.784,2.4),glm::vec3(0.749,1.3375,2.53125),glm::vec3(1.3375,0.749,2.53125),
	glm::vec3(0.805,1.4375,2.53125),glm::vec3(1.4375,0.805,2.53125),glm::vec3(0.84,1.5,2.4),glm::vec3(1.5,0.84,2.4),
	glm::vec3(1.75,0.0,1.875),glm::vec3(1.75,-0.98,1.875),glm::vec3(0.98,-1.75,1.875),glm::vec3(0.0,-1.75,1.875),
	glm::vec3(2.0,0.0,1.35),glm::vec3(2.0,-1.12,1.35),glm::vec3(1.12,-2.0,1.35),glm::vec3(0.0,-2.0,1.35),
	glm::vec3(2.0,0.0,0.9),glm::vec3(2.0,-1.12,0.9),glm::vec3(1.12,-2.0,0.9),glm::vec3(0.0,-2.0,0.9),
	glm::vec3(-0.98,-1.75,1.875),glm::vec3(-1.75,-0.98,1.875),glm::vec3(-1.75,0.0,1.875),glm::vec3(-1.12,-2.0,1.35),
	glm::vec3(-2.0,-1.12,1.35),glm::vec3(-2.0,0.0,1.35),glm::vec3(-1.12,-2.0,0.9),glm::vec3(-2.0,-1.12,0.9),
	glm::vec3(-2.0,0.0,0.9),glm::vec3(-1.75,0.98,1.875),glm::vec3(-0.98,1.75,1.875),glm::vec3(0.0,1.75,1.875),
	glm::vec3(-2.0,1.12,1.35),glm::vec3(-1.12,2.0,1.35),glm::vec3(0.0,2.0,1.35),glm::vec3(-2.0,1.12,0.9),
	glm::vec3(-1.12,2.0,0.9),glm::vec3(0.0,2.0,0.9),glm::vec3(0.98,1.75,1.875),glm::vec3(1.75,0.98,1.875),
	glm::vec3(1.12,2.0,1.35),glm::vec3(2.0,1.12,1.35),glm::vec3(1.12,2.0,0.9),glm::vec3(2.0,1.12,0.9),
	glm::vec3(2.0,0.0,0.45),glm::vec3(2.0,-1.12,0.45),glm::vec3(1.12,-2.0,0.45),glm::vec3(0.0,-2.0,0.45),
	glm::vec3(1.5,0.0,0.225),glm::vec3(1.5,-0.84,0.225),glm::vec3(0.84,-1.5,0.225),glm::vec3(0.0,-1.5,0.225),
	glm::vec3(1.5,0.0,0.15),glm::vec3(1.5,-0.84,0.15),glm::vec3(0.84,-1.5,0.15),glm::vec3(0.0,-1.5,0.15),
	glm::vec3(-1.12,-2.0,0.45),glm::vec3(-2.0,-1.12,0.45),glm::vec3(-2.0,0.0,0.45),glm::vec3(-0.84,-1.5,0.225),
	glm::vec3(-1.5,-0.84,0.225),glm::vec3(-1.5,0.0,0.225),glm::vec3(-0.84,-1.5,0.15),glm::vec3(-1.5,-0.84,0.15),
	glm::vec3(-1.5,0.0,0.15),glm::vec3(-2.0,1.12,0.45),glm::vec3(-1.12,2.0,0.45),glm::vec3(0.0,2.0,0.45),
	glm::vec3(-1.5,0.84,0.225),glm::vec3(-0.84,1.5,0.225),glm::vec3(0.0,1.5,0.225),glm::vec3(-1.5,0.84,0.15),
	glm::vec3(-0.84,1.5,0.15),glm::vec3(0.0,1.5,0.15),glm::vec3(1.12,2.0,0.45),glm::vec3(2.0,1.12,0.45),
	glm::vec3(0.84,1.5,0.225),glm::vec3(1.5,0.84,0.225),glm::vec3(0.84,1.5,0.15),glm::vec3(1.5,0.84,0.15),
	glm::vec3(-1.6,0.0,2.025),glm::vec3(-1.6,-0.3,2.025),glm::vec3(-1.5,-0.3,2.25),glm::vec3(-1.5,0.0,2.25),
	glm::vec3(-2.3,0.0,2.025),glm::vec3(-2.3,-0.3,2.025),glm::vec3(-2.5,-0.3,2.25),glm::vec3(-2.5,0.0,2.25),
	glm::vec3(-2.7,0.0,2.025),glm::vec3(-2.7,-0.3,2.025),glm::vec3(-3.0,-0.3,2.25),glm::vec3(-3.0,0.0,2.25),
	glm::vec3(-2.7,0.0,1.8),glm::vec3(-2.7,-0.3,1.8),glm::vec3(-3.0,-0.3,1.8),glm::vec3(-3.0,0.0,1.8),
	glm::vec3(-1.5,0.3,2.25),glm::vec3(-1.6,0.3,2.025),glm::vec3(-2.5,0.3,2.25),glm::vec3(-2.3,0.3,2.025),
	glm::vec3(-3.0,0.3,2.25),glm::vec3(-2.7,0.3,2.025),glm::vec3(-3.0,0.3,1.8),glm::vec3(-2.7,0.3,1.8),
	glm::vec3(-2.7,0.0,1.575),glm::vec3(-2.7,-0.3,1.575),glm::vec3(-3.0,-0.3,1.35),glm::vec3(-3.0,0.0,1.35),
	glm::vec3(-2.5,0.0,1.125),glm::vec3(-2.5,-0.3,1.125),glm::vec3(-2.65,-0.3,0.9375),glm::vec3(-2.65,0.0,0.9375),
	glm::vec3(-2.0,-0.3,0.9),glm::vec3(-1.9,-0.3,0.6),glm::vec3(-1.9,0.0,0.6),glm::vec3(-3.0,0.3,1.35),
	glm::vec3(-2.7,0.3,1.575),glm::vec3(-2.65,0.3,0.9375),glm::vec3(-2.5,0.3,1.125),glm::vec3(-1.9,0.3,0.6),
	glm::vec3(-2.0,0.3,0.9),glm::vec3(1.7,0.0,1.425),glm::vec3(1.7,-0.66,1.425),glm::vec3(1.7,-0.66,0.6),
	glm::vec3(1.7,0.0,0.6),glm::vec3(2.6,0.0,1.425),glm::vec3(2.6,-0.66,1.425),glm::vec3(3.1,-0.66,0.825),
	glm::vec3(3.1,0.0,0.825),glm::vec3(2.3,0.0,2.1),glm::vec3(2.3,-0.25,2.1),glm::vec3(2.4,-0.25,2.025),
	glm::vec3(2.4,0.0,2.025),glm::vec3(2.7,0.0,2.4),glm::vec3(2.7,-0.25,2.4),glm::vec3(3.3,-0.25,2.4),
	glm::vec3(3.3,0.0,2.4),glm::vec3(1.7,0.66,0.6),glm::vec3(1.7,0.66,1.425),glm::vec3(3.1,0.66,0.825),
	glm::vec3(2.6,0.66,1.425),glm::vec3(2.4,0.25,2.025),glm::vec3(2.3,0.25,2.1),glm::vec3(3.3,0.25,2.4),
	glm::vec3(2.7,0.25,2.4),glm::vec3(2.8,0.0,2.475),glm::vec3(2.8,-0.25,2.475),glm::vec3(3.525,-0.25,2.49375),
	glm::vec3(3.525,0.0,2.49375),glm::vec3(2.9,0.0,2.475),glm::vec3(2.9,-0.15,2.475),glm::vec3(3.45,-0.15,2.5125),
	glm::vec3(3.45,0.0,2.5125),glm::vec3(2.8,0.0,2.4),glm::vec3(2.8,-0.15,2.4),glm::vec3(3.2,-0.15,2.4),
	glm::vec3(3.2,0.0,2.4),glm::vec3(3.525,0.25,2.49375),glm::vec3(2.8,0.25,2.475),glm::vec3(3.45,0.15,2.5125),
	glm::vec3(2.9,0.15,2.475),glm::vec3(3.2,0.15,2.4),glm::vec3(2.8,0.15,2.4),glm::vec3(0.0,0.0,3.15),
	glm::vec3(0.0,-0.002,3.15),glm::vec3(0.002,0.0,3.15),glm::vec3(0.8,0.0,3.15),glm::vec3(0.8,-0.45,3.15),
	glm::vec3(0.45,-0.8,3.15),glm::vec3(0.0,-0.8,3.15),glm::vec3(0.0,0.0,2.85),glm::vec3(0.2,0.0,2.7),
	glm::vec3(0.2,-0.112,2.7),glm::vec3(0.112,-0.2,2.7),glm::vec3(0.0,-0.2,2.7),glm::vec3(-0.002,0.0,3.15),
	glm::vec3(-0.45,-0.8,3.15),glm::vec3(-0.8,-0.45,3.15),glm::vec3(-0.8,0.0,3.15),glm::vec3(-0.112,-0.2,2.7),
	glm::vec3(-0.2,-0.112,2.7),glm::vec3(-0.2,0.0,2.7),glm::vec3(0.0,0.002,3.15),glm::vec3(-0.8,0.45,3.15),
	glm::vec3(-0.45,0.8,3.15),glm::vec3(0.0,0.8,3.15),glm::vec3(-0.2,0.112,2.7),glm::vec3(-0.112,0.2,2.7),
	glm::vec3(0.0,0.2,2.7),glm::vec3(0.45,0.8,3.15),glm::vec3(0.8,0.45,3.15),glm::vec3(0.112,0.2,2.7),
	glm::vec3(0.2,0.112,2.7),glm::vec3(0.4,0.0,2.55),glm::vec3(0.4,-0.224,2.55),glm::vec3(0.224,-0.4,2.55),
	glm::vec3(0.0,-0.4,2.55),glm::vec3(1.3,0.0,2.55),glm::vec3(1.3,-0.728,2.55),glm::vec3(0.728,-1.3,2.55),
	glm::vec3(0.0,-1.3,2.55),glm::vec3(1.3,0.0,2.4),glm::vec3(1.3,-0.728,2.4),glm::vec3(0.728,-1.3,2.4),
	glm::vec3(0.0,-1.3,2.4),glm::vec3(-0.224,-0.4,2.55),glm::vec3(-0.4,-0.224,2.55),glm::vec3(-0.4,0.0,2.55),
	glm::vec3(-0.728,-1.3,2.55),glm::vec3(-1.3,-0.728,2.55),glm::vec3(-1.3,0.0,2.55),glm::vec3(-0.728,-1.3,2.4),
	glm::vec3(-1.3,-0.728,2.4),glm::vec3(-1.3,0.0,2.4),glm::vec3(-0.4,0.224,2.55),glm::vec3(-0.224,0.4,2.55),
	glm::vec3(0.0,0.4,2.55),glm::vec3(-1.3,0.728,2.55),glm::vec3(-0.728,1.3,2.55),glm::vec3(0.0,1.3,2.55),
	glm::vec3(-1.3,0.728,2.4),glm::vec3(-0.728,1.3,2.4),glm::vec3(0.0,1.3,2.4),glm::vec3(0.224,0.4,2.55),
	glm::vec3(0.4,0.224,2.55),glm::vec3(0.728,1.3,2.55),glm::vec3(1.3,0.728,2.55),glm::vec3(0.728,1.3,2.4),
	glm::vec3(1.3,0.728,2.4),glm::vec3(0.0,0.0,0.0),glm::vec3(1.5,0.0,0.15),glm::vec3(1.5,0.84,0.15),
	glm::vec3(0.84,1.5,0.15),glm::vec3(0.0,1.5,0.15),glm::vec3(1.5,0.0,0.075),glm::vec3(1.5,0.84,0.075),
	glm::vec3(0.84,1.5,0.075),glm::vec3(0.0,1.5,0.075),glm::vec3(1.425,0.0,0.0),glm::vec3(1.425,0.798,0.0),
	glm::vec3(0.798,1.425,0.0),glm::vec3(0.0,1.425,0.0),glm::vec3(-0.84,1.5,0.15),glm::vec3(-1.5,0.84,0.15),
	glm::vec3(-1.5,0.0,0.15),glm::vec3(-0.84,1.5,0.075),glm::vec3(-1.5,0.84,0.075),glm::vec3(-1.5,0.0,0.075),
	glm::vec3(-0.798,1.425,0.0),glm::vec3(-1.425,0.798,0.0),glm::vec3(-1.425,0.0,0.0),glm::vec3(-1.5,-0.84,0.15),
	glm::vec3(-0.84,-1.5,0.15),glm::vec3(0.0,-1.5,0.15),glm::vec3(-1.5,-0.84,0.075),glm::vec3(-0.84,-1.5,0.075),
	glm::vec3(0.0,-1.5,0.075),glm::vec3(-1.425,-0.798,0.0),glm::vec3(-0.798,-1.425,0.0),glm::vec3(0.0,-1.425,0.0),
	glm::vec3(0.84,-1.5,0.15),glm::vec3(1.5,-0.84,0.15),glm::vec3(0.84,-1.5,0.075),glm::vec3(1.5,-0.84,0.075),
	glm::vec3(0.798,-1.425,0.0),glm::vec3(1.425,-0.798,0.0)
};

GLuint teapot_patches[TEAPOT_PATCHES][4][4] = {
	{ { 1,2,3,4 },{ 5,6,7,8 },{ 9,10,11,12 },{ 13,14,15,16 } },
{ { 4,17,18,19 },{ 8,20,21,22 },{ 12,23,24,25 },{ 16,26,27,28 } },
{ { 19,29,30,31 },{ 22,32,33,34 },{ 25,35,36,37 },{ 28,38,39,40 } },
{ { 31,41,42,1 },{ 34,43,44,5 },{ 37,45,46,9 },{ 40,47,48,13 } },
{ { 13,14,15,16 },{ 49,50,51,52 },{ 53,54,55,56 },{ 57,58,59,60 } },
{ { 16,26,27,28 },{ 52,61,62,63 },{ 56,64,65,66 },{ 60,67,68,69 } },
{ { 28,38,39,40 },{ 63,70,71,72 },{ 66,73,74,75 },{ 69,76,77,78 } },
{ { 40,47,48,13 },{ 72,79,80,49 },{ 75,81,82,53 },{ 78,83,84,57 } },
{ { 57,58,59,60 },{ 85,86,87,88 },{ 89,90,91,92 },{ 93,94,95,96 } },
{ { 60,67,68,69 },{ 88,97,98,99 },{ 92,100,101,102 },{ 96,103,104,105 } },
{ { 69,76,77,78 },{ 99,106,107,108 },{ 102,109,110,111 },{ 105,112,113,114 } },
{ { 78,83,84,57 },{ 108,115,116,85 },{ 111,117,118,89 },{ 114,119,120,93 } },
{ { 121,122,123,124 },{ 125,126,127,128 },{ 129,130,131,132 },{ 133,134,135,136 } },
{ { 124,137,138,121 },{ 128,139,140,125 },{ 132,141,142,129 },{ 136,143,144,133 } },
{ { 133,134,135,136 },{ 145,146,147,148 },{ 149,150,151,152 },{ 69,153,154,155 } },
{ { 136,143,144,133 },{ 148,156,157,145 },{ 152,158,159,149 },{ 155,160,161,69 } },
{ { 162,163,164,165 },{ 166,167,168,169 },{ 170,171,172,173 },{ 174,175,176,177 } },
{ { 165,178,179,162 },{ 169,180,181,166 },{ 173,182,183,170 },{ 177,184,185,174 } },
{ { 174,175,176,177 },{ 186,187,188,189 },{ 190,191,192,193 },{ 194,195,196,197 } },
{ { 177,184,185,174 },{ 189,198,199,186 },{ 193,200,201,190 },{ 197,202,203,194 } },
{ { 204,204,204,204 },{ 207,208,209,210 },{ 211,211,211,211 },{ 212,213,214,215 } },
{ { 204,204,204,204 },{ 210,217,218,219 },{ 211,211,211,211 },{ 215,220,221,222 } },
{ { 204,204,204,204 },{ 219,224,225,226 },{ 211,211,211,211 },{ 222,227,228,229 } },
{ { 204,204,204,204 },{ 226,230,231,207 },{ 211,211,211,211 },{ 229,232,233,212 } },
{ { 212,213,214,215 },{ 234,235,236,237 },{ 238,239,240,241 },{ 242,243,244,245 } },
{ { 215,220,221,222 },{ 237,246,247,248 },{ 241,249,250,251 },{ 245,252,253,254 } },
{ { 222,227,228,229 },{ 248,255,256,257 },{ 251,258,259,260 },{ 254,261,262,263 } },
{ { 229,232,233,212 },{ 257,264,265,234 },{ 260,266,267,238 },{ 263,268,269,242 } },
{ { 270,270,270,270 },{ 279,280,281,282 },{ 275,276,277,278 },{ 271,272,273,274 } },
{ { 270,270,270,270 },{ 282,289,290,291 },{ 278,286,287,288 },{ 274,283,284,285 } },
{ { 270,270,270,270 },{ 291,298,299,300 },{ 288,295,296,297 },{ 285,292,293,294 } },
{ { 270,270,270,270 },{ 300,305,306,279 },{ 297,303,304,275 },{ 294,301,302,271 } }
};

//Method to fetch the control points within the supplied patch, stores them in the controls array
void fetchControlPoints(int patch, glm::vec3 controls[][4]) {
	for (int i = 0; i<4; i++) {
		for (int j = 0; j<4; j++) {
			controls[i][j] = teapot_vertices[teapot_patches[patch][i][j] - 1];
		}
	}
}

//Calculates the bezier cuve for the supplied four points and t
glm::vec3 bezierCurve(glm::vec3 points[4], float t) {
	glm::vec3 point;
	point += (points[0] * (float)(pow((1 - t), 3)));
	point += (points[1] * (float)(pow((1 - t), 2)*t * 3));
	point += (points[2] * (float)(pow(t, 2)*(1 - t) * 3));
	point += (points[3] * (float)(pow(t, 3)));
	return point;
}

//Calculates the Bezier surface points
glm::vec4 bezierSurface(float u, float v) {
	glm::vec3 points[4];
	glm::vec3 result;
	for (int i = 0; i<4; i++) {
		points[i] = bezierCurve(control_points[i], u);
	}
	result = bezierCurve(points, v);
	//Scale the results down to make them not so huge
	return glm::vec4(result.x/SCALE_FACTOR, result.y/SCALE_FACTOR, result.z/SCALE_FACTOR, 1);
}

void calculateVertices() {
	for (int p = 0; p<TEAPOT_PATCHES; p++) {
		//Fetches the current patch points
		fetchControlPoints(p, control_points);
		//Uses the grid size defined at the top
		for (int u = 0; u <= GRID_SIZE-1; u++) {
			for (int v = 0; v <= GRID_SIZE-1; v++) {
				//Pushes the new vertice onto the surface
				vertices.push_back(bezierSurface(1.0*u/(GRID_SIZE-1), 1.0*v/(GRID_SIZE-1)));
			}
		}
	}
}

void calculateIndices() {
  for (int p = 0; p < TEAPOT_PATCHES; p++) {
    for (int x = 0; x < GRID_SIZE-1; x++) {
      for (int y = 0; y < GRID_SIZE-1; y++) {
				//Calculates two triangle indices within a square
				//A: x,y
				//B: x,y+1
				//C: x+1,y+1
				//D: x+1,y
				
				//Triangle ABC
				float patch = p*pow(GRID_SIZE,2);
				indices.push_back(patch + x*GRID_SIZE + y);
				indices.push_back(patch + x*GRID_SIZE + (y+1));
				indices.push_back(patch + (x+1)*GRID_SIZE + (y+1));
				//Triangle CDA
				indices.push_back(patch + (x+1)*GRID_SIZE + (y+1));
				indices.push_back(patch + (x+1)*GRID_SIZE + y);
				indices.push_back(patch + x*GRID_SIZE + y);
			}
		}
	}
}

//Generates the Teapot vertices and indices
void generateTeapot() {
	calculateVertices();
	calculateIndices();
}

void init() {
	generateTeapot();
	GLuint vao, vbo;
	//Create and bind the VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//Create and bind the VBO for the vertices
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), &vertices[0], GL_STATIC_DRAW);
	//Create and bind the VBO for the indices
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
	//Stores the uniform value locations
	ModelView = glGetUniformLocation(program, "ModelView");
	//Enable depth testing and set the clear colour to white
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Transforms the teapot to be at VIEW_DISTANCE, and sets the rotation
	const glm::vec3 viewer_pos(0.0, 0.0, VIEW_DISTANCE);
	glm::mat4 trans, rot, model_view;
	trans = glm::translate(trans, -viewer_pos);
	rot = glm::rotate(rot, glm::radians(Theta[axisX]), glm::vec3(1, 0, 0));
	rot = glm::rotate(rot, glm::radians(Theta[axisY]), glm::vec3(0, 1, 0));
	rot = glm::rotate(rot, glm::radians(Theta[axisZ]), glm::vec3(0, 0, 1));
	model_view = trans * rot;
	//Generate the model-view matrix
	glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));
	//Draws all the points in a wireframe
	for (int i = 0; i < indices.size(); i += 3) {
		glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)));
	}
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
}
