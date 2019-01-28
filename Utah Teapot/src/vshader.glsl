/*
 * Name: Jagandeep Brar
 * Student ID: 7739801
 * Course: COMP4490 A01
 * Assignment: A02
 * Question: Q02
 * File: vshader.glsl
 */

#version 150

in vec4 vPosition;
uniform mat4 ModelView;

void main() {
	gl_Position = vPosition * ModelView;
}