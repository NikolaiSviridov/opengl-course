#version 330 core

layout (location = 0)
in vec3 in_position;
layout (location = 1)
in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float waterNormal;

out vec3 Position;
out vec3 Normal;

void main()
{
    Normal = normal;
    vec4 pos = vec4(in_position, 1.0);
    Position = in_position;
    vec4 modelPosition = model * pos;
    gl_Position = projection * view * model * pos;
    gl_ClipDistance[0] = waterNormal * modelPosition.y;
}