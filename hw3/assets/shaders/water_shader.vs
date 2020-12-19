#version 330 core

layout (location = 0)
in vec3 in_position;
layout (location = 1)
in vec2 texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Position;
out vec2 TexCoords;
out vec4 ClipCoords;
out vec4 LightPosition;

void main()
{
   vec4 pos = vec4(in_position, 1.0);
   Position = in_position;
   TexCoords = texcoords;
   ClipCoords = projection * view * model * pos;
   gl_Position = ClipCoords;
}