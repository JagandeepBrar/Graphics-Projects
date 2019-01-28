/*
	Name(s): Jagandeep Brar & Ullas Gurukumar
	Student ID(s): 7739801 & 7747220
	Course: COMP4490 A01: Computer Graphics 2

	Project: Simulated Fire & Smoke Model
	File: src/fire.cpp
	Details: Holds methods contained in fire calculation
*/

#include "fire.h"

static const glm::vec4 SPAWN_LOCATION = glm::vec4(-0.30, -1, -0.30, 1);

//Vectors for the components of the fire
std::vector<glm::vec2> fire_size = {};
std::vector<glm::vec4> fire_position = {};
std::vector<glm::vec4> fire_color = {};
std::vector<GLfloat> fire_fuel = {};
std::vector<GLfloat> fire_velocity = {};

/*
	Fire::initShader
		- Initializes the shader and sets the pointers
*/
GLuint Fire::initShader() {
	return Shaders::initShader(VSHADER_FIRE, FSHADER_FIRE);
}

/*
	Fire::useShader
		- Sets up the Fire shader, and sets the vertexes and colours buffering
*/
void Fire::useShader() {
	//Sets up some variables, initializes the program
	assert(Glut::program[fire_object] != NULL);
	GLuint vPosition, vColor, vSize;
	//Set to use the program
	glUseProgram(Glut::program[fire_object]);
	//Sets up the vector of vecrtices for the buffer
	vPosition = glGetAttribLocation(Glut::program[fire_object], "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	//Sets up the vector of colors for the buffer
	vColor = glGetAttribLocation(Glut::program[fire_object], "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(VEC4_VECTOR_SIZE(fire_position)));
	//Sets up the vector of sizes for the buffer
	vSize = glGetAttribLocation(Glut::program[fire_object], "vSize");
	glEnableVertexAttribArray(vSize);
	glVertexAttribPointer(vSize, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(VEC4_VECTOR_SIZE(fire_position)+VEC4_VECTOR_SIZE(fire_color)));
	//Setup the Uniform values
	Glut::modelView = glGetUniformLocation(Glut::program[fire_object], "modelView");
	Glut::adjustModelView();
} 

/*
	Fire::drawParticles
		- Enables the correct shader and draws the particles
*/
void Fire::drawParticles() {
	int size_particles = VEC4_VECTOR_SIZE(fire_position), size_colors = VEC4_VECTOR_SIZE(fire_color), size_sizes = VEC2_VECTOR_SIZE(fire_size);
	//Bind and update the VAO & VBO
	glBufferData(GL_ARRAY_BUFFER, size_particles + size_colors + size_sizes, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size_particles, &fire_position[0]);
	glBufferSubData(GL_ARRAY_BUFFER, size_particles, size_colors, &fire_color[0]);
	glBufferSubData(GL_ARRAY_BUFFER, size_particles+size_colors, size_sizes, &fire_size[0]);
	//Swap the shader data
	Fire::useShader();
	glDrawArrays(GL_POINTS, 0, (size_particles/sizeof(glm::vec4)));
}

/*
	Fire::refreshParticles
		- Refreshes the particle data
*/
void Fire::refreshParticles() {
	//Step through all particles, and apply modifications
	for (int p=0; p<(int)fire_position.size(); p++) {
		int state = 0;
		Fire::adjustColor(p);
		state -= Fire::adjustFuel(p);
		state -= Fire::adjustPosition(p);
		state -= Fire::adjustSize(p);
		//If any of the states returned 1 (dead), state is less than one, and we have to remove the particle
		if(state < 0) {
			Fire::removeParticle(p);
			p--;
		}
	}
	//Spawn new particles
	spawnParticles();
}

/*
	Fire::adjustColor
		- Adjusts the color of the particle location
*/
GLuint Fire::adjustColor(int location) {
	//Fetches current color and gas addresses, as they relate
	glm::vec4 *col = &fire_color[location];
	const GLfloat *gas = &fire_fuel[location];
	//Depending on the level of gas, set it to white, red or yellow
	if ((*gas) > 40) {
		*col = COLOR_WHITE;
	}
	else if ((*gas) > 20) {
		*col = COLOR_YELLOW;
	}
	else {
		*col = COLOR_RED;
	}
	//Gets an RNG alpha value
	col->a = (float)1/(rand() % 10);
	return state_alive;
}

/*
	Fire::adjustFuel
		- Adjusts the fuel of the particle location
*/
GLuint Fire::adjustFuel(int location) {
	GLfloat *gas = &fire_fuel[location];
	(*gas)--;
	if (*gas <= 0) {
		return state_dead;
	}
	return state_alive;
}

/*
	Fire::adjustPosition
		- Adjusts the position of the particle location
*/
GLuint Fire::adjustPosition(int location) {
	glm::vec4 *pos = &fire_position[location];
	GLfloat *vel = &fire_velocity[location];
	pos->y += (*vel)/(float)FIRE_SMOKE_RESOLUTION;
	//Does an out of bounds check and kills the particle if it is out of bounds
	if (pos->x >= 1 || pos->y >= 1 || pos->z >= 1 || pos->x <= -1 || pos->y <= -1 || pos->z <= -1) {
		return state_dead;
	}
	return state_alive;
}

/*
	Fire::adjustSize
		- Adjusts the size of the particle location
*/
GLuint Fire::adjustSize(int location) {
	glm::vec2 *size = &fire_size[location];
	size->x -= 1;
	//Does a size check, if it is zero (or lower), kill it
	if (size->x <= 0) {
		return state_dead;
	}
	return state_alive;
}

/*
	Fire::spawnParticles
		- Spawns "FIRE_PARTICLES_PER_FRAME" amount of new fire particles at the spawn location
*/
void Fire::spawnParticles() {
	for (int i=0; i<FIRE_PARTICLES_PER_FRAME; i++) {
		//Uses RNG to calculate different values
		glm::vec4 
			spawn = { SPAWN_LOCATION.x+(i *0.025), SPAWN_LOCATION.y, SPAWN_LOCATION.z, SPAWN_LOCATION.w };
		float 
			size = ((float)FIRE_MIN_INITIAL_SIZE + (rand() % FIRE_MAX_INITIAL_SIZE)),
			gas = ((float)FIRE_MIN_INITIAL_FUEL + (rand() % FIRE_MAX_INITIAL_FUEL)),
			velocity = ((float)FIRE_MIN_INITIAL_VELOCITY + (rand() % FIRE_MAX_INITIAL_VELOCITY));
		//Creates a new data entry at the back of each vector
		fire_position.push_back(spawn);
		fire_color.push_back(COLOR_WHITE);
		fire_size.push_back(glm::vec2(size, 0));
		fire_velocity.push_back(velocity);
		fire_fuel.push_back(gas);
	}
}

/*
	Fire::removeLocation
		- Removes all particle data from all vectors at the location supplied
*/
void Fire::removeParticle(int location) {
	//Erases the data entry in all the vectors
	Smoke::spawnParticles(fire_position[location]);
	fire_position.erase(fire_position.begin()+location);
	fire_color.erase(fire_color.begin()+location);
	fire_size.erase(fire_size.begin()+location);
	fire_velocity.erase(fire_velocity.begin()+location);
	fire_fuel.erase(fire_fuel.begin() + location);
}
