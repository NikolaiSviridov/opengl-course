#version 330 core

layout (location = 0)
in vec3 in_position;
layout (location = 1)
in vec3 normal;
layout (location = 2)
in vec2 texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float waterNormal;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    Normal = normal;
    TexCoords = texcoords;
    vec4 pos = vec4(in_position, 1.0);
    Position = in_position;
    vec4 modelPosition = model * pos;
    gl_Position = projection * view * model * pos;
    gl_ClipDistance[0] = waterNormal * (modelPosition.y + 0.01);
}
