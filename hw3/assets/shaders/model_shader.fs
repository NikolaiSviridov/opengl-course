#version 330 core

out vec4 o_frag_color;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 in_diffuse;
uniform vec3 in_specular;
uniform vec3 in_ambient;
uniform sampler2D texture_diffuse1;
uniform vec3 sunPosition;
uniform vec3 cameraPosition;

void main()
{
    float ambientStrength = 0.5;
    vec3 sunColor = vec3(1, 1, 1);
    vec3 projectorColor = vec3(1, 1, 0);
    float sunAttenuation = 1;

    vec3 sunAmbient = (ambientStrength + in_ambient) * sunColor;
    vec3 sunDirection = normalize(sunPosition);
    vec3 sunDiffuse = max(dot(normalize(Normal), sunDirection), 0.0) * in_diffuse * sunColor;

    vec3 I = normalize(cameraPosition - Position);
    vec3 sunSpecular = pow(max(dot(I, reflect(-sunDirection, normalize(Normal))), 0.0), 16) * in_specular * sunColor;

    o_frag_color = vec4(sunAmbient + sunDiffuse + sunSpecular, 1.0) * texture(texture_diffuse1, TexCoords);
}
