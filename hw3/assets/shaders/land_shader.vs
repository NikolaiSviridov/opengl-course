#version 330 core

layout (location = 0)
in vec3 in_position;
layout (location = 1)
in vec2 in_texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix1;
uniform mat4 lightSpaceMatrix2;
uniform mat4 lightSpaceMatrix3;

uniform float waterNormal;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoords;
out vec4 LightPos1;
out vec4 LightPos2;
out vec4 LightPos3;
out float z;

void main()
{
    TexCoords = in_texcoords;
    vec4 pos = vec4(in_position, 1.0);
    Position = in_position;
    vec4 modelPosition = model * pos;
    LightPos1 = lightSpaceMatrix1 * modelPosition;
    LightPos2 = lightSpaceMatrix2 * modelPosition;
    LightPos3 = lightSpaceMatrix3 * modelPosition;
    gl_Position = projection * view * modelPosition;
    z = gl_Position.z;
    gl_ClipDistance[0] = waterNormal * modelPosition.y;
}