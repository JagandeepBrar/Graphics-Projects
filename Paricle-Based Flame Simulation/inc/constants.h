/*
	Name(s): Jagandeep Brar & Ullas Gurukumar
	Student ID(s): 7739801 & 7747220
	Course: COMP4490 A01: Computer Graphics 2

	Project: Simulated Fire & Smoke Model
	File: inc/constants.h
	Details: Holds any and all constants/macro definitions
*/

#pragma once

//General Definitions
#define OPENGL_MAJOR_VER 4
#define OPENGL_MINOR_VER 5
#define WINDOW_SIZE 512
#define WINDOW_TITLE "Particle Engine-Based Fire & Smoke Simulation"
#define FPS_TARGET 60
//View-Related Definitions
#define VIEW_DISTANCE 0
#define ROTATION_SPEED 3
//Smoke & Fire-Related Definitions
#define FIRE_SMOKE_RESOLUTION 128
//Fire-Related Definitions
#define FIRE_MIN_INITIAL_VELOCITY 0.1
#define FIRE_MIN_INITIAL_SIZE 10
#define FIRE_MIN_INITIAL_FUEL 20
#define FIRE_MAX_INITIAL_VELOCITY 3
#define FIRE_MAX_INITIAL_SIZE 50
#define FIRE_MAX_INITIAL_FUEL 40
#define FIRE_MAX_FUEL (FIRE_MIN_INITIAL_FUEL+FIRE_MAX_INITIAL_FUEL)
#define FIRE_PARTICLES_PER_FRAME 15
//Smoke-Related Definitions
#define SMOKE_MIN_INITIAL_VELOCITY 0.05
#define SMOKE_MIN_INITIAL_SIZE 10
#define SMOKE_MIN_INITIAL_FUEL 10
#define SMOKE_MAX_INITIAL_VELOCITY 2
#define SMOKE_MAX_INITIAL_SIZE 15
#define SMOKE_MAX_INITIAL_FUEL 75
#define SMOKE_MAX_FUEL (SMOKE_MIN_INITIAL_FUEL+SMOKE_MAX_INITIAL_FUEL)
#define SMOKE_PARTICLES_PER_FIRE 1
//Shader-Related Definitions
#define VSHADER_FIRE "data/fire_vshader.glsl"
#define VSHADER_SMOKE "data/smoke_vshader.glsl"
#define FSHADER_FIRE "data/fire_fshader.glsl"
#define FSHADER_SMOKE "data/smoke_fshader.glsl"
//Macro Definitions
#define BUFFER_OFFSET(offset) ((GLvoid*) (offset))
#define FPS_TO_MS ((unsigned int) (1000/FPS_TARGET))
#define AXIS_ARRAY_LOC(axis) ((unsigned int)(std::abs(axis)-1))
#define VEC1_VECTOR_SIZE(vec) ((unsigned int)(vec.size()*sizeof(glm::vec1)))
#define VEC2_VECTOR_SIZE(vec) ((unsigned int)(vec.size()*sizeof(glm::vec2)))
#define VEC3_VECTOR_SIZE(vec) ((unsigned int)(vec.size()*sizeof(glm::vec3)))
#define VEC4_VECTOR_SIZE(vec) ((unsigned int)(vec.size()*sizeof(glm::vec4)))
//Enumerator Definitions
enum axes {
	axis_x = 1,
	axis_y = 2,
	axis_z = 3
};
enum objects {
	fire_object,
	smoke_object,
	total_objects
};
enum states {
	state_alive,
	state_dead
};
