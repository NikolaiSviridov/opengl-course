#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 texture_coords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform float c_texture;
uniform float c_reflection;
uniform float c_refraction;
uniform float fresnel;
uniform float frag_color;

void main()
{
    vec4 diffuse = texture2D(texture_diffuse1, texture_coords);
    vec4 specular = texture2D(texture_specular1, texture_coords);
    vec4 result_t = mix(diffuse, specular, c_texture);

    vec3 reflection = reflect(normalize(Position - cameraPos), normalize(Normal));
    vec3 refraction = refract(normalize(Position - cameraPos), normalize(Normal), c_refraction);
    float angle = -dot(normalize(Position - cameraPos), normalize(Normal));

    vec4 R = mix(
        vec4(texture(skybox, refraction).rgb, 1.0),
        vec4(texture(skybox, reflection).rgb, 1.0),
        pow(1 - angle, fresnel) * (1 - c_reflection) + c_reflection
    );

    FragColor = mix(R, result_t, frag_color);
}
