#version 420 core
layout (location = 0) in vec3 aPos;

const int NUM_CASCADES = 6;

//uniform mat4 lightSpaceMatrix[NUM_CASCADES];
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    //gl_Position = lightSpaceMatrix[0] * model * vec4(aPos, 1.0);
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}