#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

const int NUM_CASCADES = 6;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[NUM_CASCADES];
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix[NUM_CASCADES];

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    for (int i = 0; i < NUM_CASCADES; ++i)
    {
        vs_out.FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    }
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}