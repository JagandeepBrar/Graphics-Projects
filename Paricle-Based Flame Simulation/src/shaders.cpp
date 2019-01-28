/*
	Name(s): Jagandeep Brar & Ullas Gurukumar
	Student ID(s): 7739801 & 7747220
	Course: COMP4490 A01: Computer Graphics 2

	Project: Simulated Fire & Smoke Model
File: src/shaders.cpp
Details: Holds methods to load shaders into the program
*/

#include "shaders.h"

static FILE *vertex_file;
static FILE *fragment_file;

/*
	Shaders::initShader
		- Initializes the vertex and fragment shader files that were passed as arguments
*/
GLuint Shaders::initShader(const char* vertex, const char* fragment) {
	//Create a new program, and open the shader files
	GLuint program = glCreateProgram();
	openShaderFiles(vertex, fragment);
	//Load, attach, and link the shaders
	attachShader(program, vertex_file, GL_VERTEX_SHADER);
	attachShader(program, fragment_file, GL_FRAGMENT_SHADER);
	linkProgram(program);
	//Close the files, and return the program
	closeShaderFiles();
	return program;
}

/*
	Shaders::openShaderFiles
		- Opens the vertex and fragment shaders as binary files
		- Stores the two open files globally
*/
void Shaders::openShaderFiles(const char* vertex, const char* fragment) {
	int v_err = fopen_s(&vertex_file, vertex, "rb");
	int f_err = fopen_s(&fragment_file, fragment, "rb");
	//Asserts the opens, to ensure it correctly opened
	assert(v_err == 0 && f_err == 0);
}

/*
	Shaders::closeShaderFiles
		- Closes the open vertex and fragment shader files
*/
void Shaders::closeShaderFiles() {
	int v_err = fclose(vertex_file);
	int f_err = fclose(fragment_file);
	//Asserts the closes, to ensure they correctly closed
	assert(v_err == 0 && f_err == 0);
}

/*
	Shaders::linkProgram
		- Links the shader program to OpenGL
*/
void Shaders::linkProgram(GLuint program) {
	//Link the program
	glLinkProgram(program);
	//Checks for a successful link
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	assert(linked);
}

/*
	Shaders::attachShader
		- Compiles the supplied shader, and attaches it to the program
*/
void Shaders::attachShader(GLuint program, FILE *file, GLenum type) {
	//Get the vertex shader data, and create a new shader
	GLint compiled;
	GLchar *data = getFileBytes(file);
	GLuint shader = glCreateShader(type);
	//Load the data into the shader and compile
	glShaderSource(shader, 1, &data, NULL);
	glCompileShader(shader);
	//Ensure the shader compiled successfully
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	assert(compiled);
	//Attach the compiled shader to the program
	glAttachShader(program, shader);
}

/*
	Shaders::getFileSize
		- Returns the shader size in bytes of the supplied shader
*/
long Shaders::getFileSize(FILE *file) {
	//Seeks to the end, returns the distance (size)
	fseek(file, 0, SEEK_END);
	return (ftell(file));
}

/*
	Shaders::getFileBytes
		- Returns the byte data of the supplied shader
*/
char* Shaders::getFileBytes(FILE *file) {
	//Gets the size, and makes a data buffer using the size
	long size = getFileSize(file);
	char* data = new char[size + 1];
	//Reads the entire file into memory, and appends a null terminator
	fseek(file, 0, SEEK_SET);
	fread(data, 1, size, file);
	data[size] = '\0';
	//Returns the final buffer
	return data;
}
