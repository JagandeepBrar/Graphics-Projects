/*
	Name(s): Jagandeep Brar & Ullas Gurukumar
	Student ID(s): 7739801 & 7747220
	Course: COMP4490 A01: Computer Graphics 2

	Project: Simulated Fire & Smoke Model
	File: inc/shaders.h
	Details: Header for the Shaders class
*/

#pragma once

#include "constants.h"
#include "libraries.h"

class Shaders {
	private:
		static void openShaderFiles(const char* vertex, const char* fragment);
		static void closeShaderFiles();
		static void linkProgram(GLuint program);
		static void attachShader(GLuint program, FILE *file, GLenum type);
		static long getFileSize(FILE *file);
		static char* getFileBytes(FILE *file);
	public:
		static GLuint initShader(const char* vertex, const char* fragment);
};
