/*
	Name(s): Jagandeep Brar & Ullas Gurukumar
	Student ID(s): 7739801 & 7747220
	Course: COMP4490 A01: Computer Graphics 2

	Project: Simulated Fire & Smoke Model
	File: src/smoke.cpp
	Details: Holds methods contained in smoke calculation
*/

#include "smoke.h"

//Vectors for the components of the smoke
std::vector<glm::vec2> smoke_size = {};
std::vector<glm::vec4> smoke_position = {};
std::vector<glm::vec4> smoke_color = {};
std::vector<GLfloat> smoke_velocity = {};
std::vector<GLfloat> smoke_fuel = {};

/*
	Smoke::initShader
		- Initializes the shader and sets the pointers
*/
GLuint Smoke::initShader() {
	return Shaders::initShader(VSHADER_SMOKE, FSHADER_SMOKE);
}

/*
	Smoke::useShader
		- Sets the vertexes and colours buffering
*/
void Smoke::useShader() {
	//Sets up some variables
	assert(Glut::program[smoke_object] != NULL);
	GLuint vPosition, vColor, vSize;
	//Set to use the program
	glUseProgram(Glut::program[smoke_object]);
	//Sets up the vector of vecrtices for the buffer
	vPosition = glGetAttribLocation(Glut::program[smoke_object], "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	//Sets up the vector of colors for the buffer
	vColor = glGetAttribLocation(Glut::program[smoke_object], "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(VEC4_VECTOR_SIZE(smoke_position)));
	//Sets up the vector of sizes for the buffer
	vSize = glGetAttribLocation(Glut::program[smoke_object], "vSize");
	glEnableVertexAttribArray(vSize);
	glVertexAttribPointer(vSize, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(VEC4_VECTOR_SIZE(smoke_position)+VEC4_VECTOR_SIZE(smoke_color)));
	//Setup the Uniform values
	Glut::modelView = glGetUniformLocation(Glut::program[smoke_object], "modelView");
	Glut::adjustModelView();
}

/*
	Smoke::drawParticles
		- Enables the correct shader and draws the particles
*/
void Smoke::drawParticles() {
	int size_particles = VEC4_VECTOR_SIZE(smoke_position), size_colors = VEC4_VECTOR_SIZE(smoke_color), size_sizes = VEC2_VECTOR_SIZE(smoke_size);
	//Bind and update the VAO & VBO
	if (smoke_position.size() > 0) {
		glBufferData(GL_ARRAY_BUFFER, size_particles + size_colors + size_sizes, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size_particles, &smoke_position[0]);
		glBufferSubData(GL_ARRAY_BUFFER, size_particles, size_colors, &smoke_color[0]);
		glBufferSubData(GL_ARRAY_BUFFER, size_particles + size_colors, size_sizes, &smoke_size[0]);
		//Swap the shader data
		Smoke::useShader();
		glDrawArrays(GL_POINTS, 0, (size_particles / sizeof(glm::vec4)));
	}
}

/*
	Smoke::refreshParticles
		- Refreshes the smoke data
*/
void Smoke::refreshParticles() {
	//Step through all particles and apply modifications
	for(int p=0; p<(int)smoke_position.size();p++) {
		int state = 0;
		state -= Smoke::adjustColor(p);
		state -= Smoke::adjustFuel(p);
		state -= Smoke::adjustPosition(p);
		state -= Smoke::adjustSize(p);
		if(state < 0) {
			Smoke::removeParticle(p);
			p--;
		}
	}
}

/*
	Smoke::adjustColor
		- Adjusts the color of the particle location
*/
GLuint Smoke::adjustColor(int location) {
	return state_alive;
}

/*
	Smoke::adjustFuel
		- Adjusts the fuel of the particle location
*/
GLuint Smoke::adjustFuel(int location) {
	GLfloat *gas = &smoke_fuel[location];
	(*gas)--;
	if (*gas <= 0) {
		return state_dead;
	}
	return state_alive;
}

/*
	Smoke::adjustPosition
		- Adjusts the position of the particle location
*/
GLuint Smoke::adjustPosition(int location) {
	glm::vec4 *pos = &smoke_position[location];
	GLfloat *vel = &smoke_velocity[location];
	pos->y += (*vel)/(float)FIRE_SMOKE_RESOLUTION;
	//Does an out of bounds check and kills the particle if it is out of bounds
	if (pos->x >= 1 || pos->y >= 1 || pos->z >= 1 || pos->x <= -1 || pos->y <= -1 || pos->z <= -1) {
		return state_dead;
	}
	return state_alive;
}

/*
	Smoke::adjustSize
		- Adjusts the size of the particle location
*/
GLuint Smoke::adjustSize(int location) {
	glm::vec2 *size = &smoke_size[location];
	size->x -= 0.25;
	//Does a size check, if it is zero (or lower), kill it
	if (size->x <= 0) {
		return state_dead;
	}
	return state_alive;
}

/*
	Smoke::spawnParticles
		- Spawns "SMOKE_PARTICLES_PER_FIRE" of new smoke particles at the position supplied
*/
void Smoke::spawnParticles(glm::vec4 position) {
	for(int i=0; i<SMOKE_PARTICLES_PER_FIRE; i++) {
		//Uses RNG to calculate different values
		float 
			size = ((float)SMOKE_MIN_INITIAL_SIZE + (rand() % SMOKE_MAX_INITIAL_SIZE)),
			gas = ((float)SMOKE_MIN_INITIAL_FUEL + (rand() % SMOKE_MAX_INITIAL_FUEL)),
			velocity = ((float)SMOKE_MIN_INITIAL_VELOCITY + (rand() % SMOKE_MAX_INITIAL_VELOCITY));
		//Creates a new smoke particle in all the vector components
		smoke_position.push_back(position);
		smoke_color.push_back(COLOR_GREY);
		smoke_size.push_back(glm::vec2(size, 0));
		smoke_velocity.push_back(velocity);
		smoke_fuel.push_back(gas);
	}
}

/*
	smoke::removeLocation
		- Removes all particle data from all vectors at the location supplied
*/
void Smoke::removeParticle(int location) {
	//Erases the data entry in all the vectors
	smoke_position.erase(smoke_position.begin()+location);
	smoke_color.erase(smoke_color.begin()+location);
	smoke_size.erase(smoke_size.begin()+location);
	smoke_velocity.erase(smoke_velocity.begin()+location);
	smoke_fuel.erase(smoke_fuel.begin() + location);
}
