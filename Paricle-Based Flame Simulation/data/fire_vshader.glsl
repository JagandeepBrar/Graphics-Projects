/*
	Name(s): Jagandeep Brar & Ullas Gurukumar
	Student ID(s): 7739801 & 7747220
	Course: COMP4490 A01: Computer Graphics 2

	Project: Simulated Fire & Smoke Model
	File: data/fire_vshader.glsl
	Details: The vertex shader for the fire
*/

#version 450

in vec4 vPosition;
in vec4 vColor;
in vec2 vSize;
out vec4 fColor;
uniform mat4 modelView;

void main() {
	gl_Position = vPosition * modelView;
	gl_PointSize = vSize.x;
	fColor = vColor;
}
