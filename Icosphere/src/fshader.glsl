/*
 * Name: Jagandeep Brar
 * Student ID: 7739801
 * Course: COMP4490 A01
 * Assignment: A02
 * Question: Q01
 * File: fshader.glsl
 * 
 * Using the supplied OpenGL code (sepcifically example6.cpp):
 * Create an icosphere with half being perturbed in the vshader
 */

#version 150

flat in float randomizer;
out vec4 color;
uniform int Border;

void main() 
{
  //If the border uniform value is 1, it's the border so always make it black
  if(Border == 1) {
    color = vec4(0,0,0,1);
  //If the randomizer is 9999, it means that it is the regular sphere part, set it to a grey colour
  } else if(randomizer == 9999) {
    color = vec4(0.20,0.20,0.20,1);
  } else {
  //Else if it is the perturbed part, use the randomizer passed through to calculate the colour
  //The base vec4 is a blue colour, randomizer is multipled to vary the colours
    color = vec4(0.53725,0.81176,0.94118,1)*randomizer;
  }
}