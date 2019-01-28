/*
	Name(s): Jagandeep Brar & Ullas Gurukumar
	Student ID(s): 7739801 & 7747220
	Course: COMP4490 A01: Computer Graphics 2

	Project: Simulated Fire & Smoke Model
	File: inc/fire.h
	Details: Header for the Fire class
*/

#pragma once

#include "colors.h"
#include "constants.h"
#include "libraries.h"

class Fire {
	public:
		static GLuint initShader();
		static void useShader();
		static void drawParticles();
		static void refreshParticles();
		static void spawnParticles();
		static void removeParticle(int location);
		//Adjustment Methods
		static GLuint adjustColor(int location);
		static GLuint adjustFuel(int location);
		static GLuint adjustPosition(int location);
		static GLuint adjustSize(int location);
};
