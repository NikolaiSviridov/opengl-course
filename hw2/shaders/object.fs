#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 in_c_texture;

uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform float c_reflection;
uniform float c_refraction;
uniform float fresnel;

void main()
{    
    float angle = -dot(normalize(Position - cameraPos), normalize(Normal));
    vec3 reflection = reflect(normalize(Position - cameraPos), normalize(Normal));
    vec3 refraction = refract(normalize(Position - cameraPos), normalize(Normal), c_refraction);
    float coefficient_fresnel = pow(1 - angle, fresnel) * (1 - c_reflection) + c_reflection;

    FragColor = mix(
                    vec4(texture(skybox, refraction).rgb, 1.0)
                    , vec4(texture(skybox, reflection).rgb, 1.0)
                    , coefficient_fresnel
                    );
}