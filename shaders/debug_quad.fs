#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap[6];
uniform float near_plane;
uniform float far_plane;


void main()
{

    float depthValue = texture(depthMap[2], TexCoords).r;
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}