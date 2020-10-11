#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 in_c_texture;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform float c_reflection;
uniform float c_refraction;
uniform float fresnel_alpha;
uniform float frag_color_mix;

void main()
{    
    vec4 diffuse = texture2D(texture_diffuse1, in_c_texture);
    vec4 specular = texture2D(texture_specular1, in_c_texture);
    vec4 result_texture = mix(diffuse, specular, 0);

    float angle = -dot(normalize(Position - cameraPos), normalize(Normal));
    vec3 reflection = reflect(normalize(Position - cameraPos), normalize(Normal));
    vec3 refraction = refract(normalize(Position - cameraPos), normalize(Normal), c_refraction);
    float coefficient_fresnel = pow(1 - angle, fresnel_alpha) * (1 - c_reflection) + c_reflection;

    vec4 R = mix(
        vec4(texture(skybox, refraction).rgb, 1.0)
        , vec4(texture(skybox, reflection).rgb, 1.0)
        , coefficient_fresnel
         );

    FragColor = mix(R, result_texture, 0);
}