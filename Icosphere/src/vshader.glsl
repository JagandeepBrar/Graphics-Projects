/*
 * Name: Jagandeep Brar
 * Student ID: 7739801
 * Course: COMP4490 A01
 * Assignment: A02
 * Question: Q01
 * File: vshader.glsl
 * 
 * Using the supplied OpenGL code (sepcifically example6.cpp):
 * Create an icosphere with half being perturbed in the vshader
 */

#version 150

in vec4 vPosition;
flat out float randomizer;
uniform mat4 ModelView, Projection;

float rand(vec2 co);
void normal();
void perturb();

void main() {
    if(vPosition.x > 0.23456) {
        perturb();
    } else {
        normal();
    }
}

//For regular rendering of the sphere
void normal() {
    //Set position regularly, and set the randomizer to 9999
    gl_Position = Projection * ModelView * vPosition;
    randomizer = 9999;
}

//For rendering the perturbed part of the sphere
void perturb() {
    //Changes the vPosition slightly using the supplied random number generator
    vec4 pos = vPosition.xyzw;
    randomizer = rand(vPosition.xy);
    pos.x = pos.x-randomizer/15;
    //Set the Position using new calculated position
    gl_Position = Projection * ModelView * pos;
}

//Supplied in the outline, a random number generator
float rand(vec2 co) {
    return fract(sin(dot(co.xy,vec2(12.9898,78.233)))*43758.5453);
}
